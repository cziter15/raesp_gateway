/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the RaespDevice (WiFi 433 MHz bridge) firmware.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#include "RadioCommander.h"
#include <RadioLib.h>

#include "board.h"
#include "../protocols/nexa.h"
#include "../protocols/ningbo.h"

using namespace std::placeholders;

namespace apps::raesp::comps
{
	RadioCommander::RadioCommander(uint8_t ssPin, uint8_t dio0pin, uint8_t rstPin, uint8_t dio2pin, ksLedWP wifiLedWp, ksLedWP radioLedWp)
		: radioLedWp(radioLedWp), wifiLedWp(wifiLedWp)
	{
		/* Instantiate radio PHY/Module. */
		radioPhy = std::make_shared<Module>(ssPin, dio0pin, rstPin, dio2pin);
		radioModule = std::make_shared<SX1278>(radioPhy.get());

		/* Setup radio module. */
		cachedFrequency = TRANSMIT_FREQ_NEXA;
		radioModule->beginFSK(cachedFrequency, 4.8, 5.0, 125.0, TRANSMIT_POWER_DBM, 8, true);

		/* Setup radio TX pin. */
		#if TX_WITH_PULLUP
			pinMode(dio2pin, INPUT_PULLUP);
			GPES = (1 << dio2pin);
		#else
			pinMode(dio2pin, OUTPUT);
		#endif
	}

	bool RadioCommander::init(ksf::ksApplication* owner)
	{
		mqttConnWp = owner->findComponent<ksf::comps::ksMqttConnector>();

		if (auto mqttConnSp{mqttConnWp.lock()})
		{
			mqttConnSp->onConnected->registerEvent(connEventHandleSp, std::bind(&RadioCommander::onMqttConnected, this));
			mqttConnSp->onMesssage->registerEvent(msgEventHandleSp, std::bind(&RadioCommander::onMqttMessage, this, _1, _2));
		}
	
		return true;
	}

	void RadioCommander::onMqttConnected()
	{
		if (auto mqttConnSp{mqttConnWp.lock()})
			mqttConnSp->subscribe(rfTopicPrefix + '#');
	}

	void RadioCommander::sendMqttInfo(const std::string& info) const
	{
		if (auto mqttConnSp{mqttConnWp.lock()})
			mqttConnSp->publish(PGM_("log"), info);
	}

	void RadioCommander::onMqttMessage(const std::string_view& topic, const std::string_view& payload)
	{
		if (topic.find(rfTopicPrefix) == std::string::npos)
			return;

		/* Radio commander payload should be one char ('1' or '0') */
		if (payload.length() != 1)
			return;
			
		/* Check command queue overflow. */
		if (commandQueue.size() > MAX_TX_QUEUE_SIZE)
		{
			/* If overflow, then blink RED wifi LED. */
			if (auto wifiLedSp = wifiLedWp.lock())
				if (!wifiLedSp->isBlinking())
					wifiLedSp->setBlinking(100, 5);

			/* ... and tell MQTT via log channel about that. */
			sendMqttInfo(PGM_("RadioCmd: Queue is full - discarding!"));
			return;
		}

		/*	
			Try split topic. Based of delimeter presence specific protocol is selected.

			Topics like OOK/1234/16 will be handled by nexa protocol.
			Topics like OOK/1 will be handled by ningbo protocol.
		*/
		auto delim_idx{topic.find('/', rfTopicPrefix.length() + 1)};

		uint32_t address{0};
		int16_t unit{RC_UNIT_NONE};
		
		if (delim_idx != std::string::npos)
		{
			auto address_sv{topic.substr(rfTopicPrefix.length(), delim_idx - rfTopicPrefix.length())};
			if (!ksf::from_chars(address_sv, address))
				return;

			auto unit_sv{topic.substr(delim_idx + 1)};
			if (!ksf::from_chars(unit_sv, unit))
				return;
		}
		else
		{
			auto address_sv{topic.substr(rfTopicPrefix.length())};
			if (!ksf::from_chars(address_sv, address))
				return;
		}

		/* 
			If command queue is empty, we are in standby mode.
			We should set radio module to transmit mode and queue new request to send over air.
			Otherwise, we should have already transmit mode selected, so only queue in that case.
		*/
		if (commandQueue.empty())
			radioModule->transmitDirect();

		/* Push address/unit to command queue. */
		commandQueue.emplace(payload[0] == '1', address, unit, (uint8_t)(unit > 0 ? 6 : 9));
	}

	void RadioCommander::forceStandby()
	{
		/* Erase command queue. */
		commandQueue = {};

		/* Set randio module standby state. */
		if (radioModule)
			radioModule->standby();
	}

	void RadioCommander::changeFrequency(double frequency)
	{
		if (cachedFrequency != frequency)
		{
			cachedFrequency = frequency;
			radioModule->setFrequency(frequency);
		}
	}

	void RadioCommander::processRadioCommand(RadioCommand &command)
	{
		if (auto radioLedSp{radioLedWp.lock()})
		{
			/* Here we decide if we use ningbo protocol or nexa protocol. */
			if (command.unit == RC_UNIT_NONE)
				protocols::tx_ningbo_switch({radioPhy->getGpio(), radioLedSp->getPin()}, command.enable, command.address);
			else
				protocols::tx_nexa_switch({radioPhy->getGpio(), radioLedSp->getPin()}, command.enable, command.address, command.unit);
		}

		command.repeats--;
	}

	bool RadioCommander::loop()
	{
		if (!commandQueue.empty())
		{
			/*
				Handle single 'repeat' of OOK message request. 
				One request per application loop.
			*/
			auto& currentCommand{commandQueue.front()};
			processRadioCommand(currentCommand);
			
			/* Check if it's last repeat. */
			if (currentCommand.repeats <= 0)
			{
				sendMqttInfo(
					PGM_("RadioCmd: Sent! "
					"[ A: ") + ksf::to_string(currentCommand.address) + 
					PGM_(" | U: ") + ksf::to_string(currentCommand.unit) + 
					PGM_(" | V: ") + ksf::to_string(currentCommand.enable) + PGM_(" ]")
				);

				/* Pop current request (remove) from queue, coz we are done with it. */
				commandQueue.pop();
				
				/* If command queue is empty now, we can set RF module to standby state. */
				if (commandQueue.empty())
					radioModule->standby();
			}
		}

		return true;
	}
}