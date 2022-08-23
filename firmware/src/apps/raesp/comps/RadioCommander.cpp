#include "RadioCommander.h"
#include <RadioLib.h>
#include "board.h"

#include "../protocols/nexa.h"
#include "../protocols/ningbo.h"

using namespace std::placeholders;

namespace raesp::comps
{
	RadioCommander::RadioCommander(uint8_t ssPin, uint8_t dio0pin, uint8_t rstPin, uint8_t dio2pin, ksLedWP wifiLedWp, ksLedWP radioLedWp)
		: radioLedWp(radioLedWp), wifiLedWp(wifiLedWp)
	{
		/* Instantiate radio PHY/Module. */
		radioPhy = std::make_shared<Module>(ssPin, dio0pin, rstPin, dio2pin);
		radioModule = std::make_shared<SX1278>(radioPhy.get());

		/* Setup radio module. */
		radioModule->beginFSK(TRANSMIT_FREQ, 4.8, 5.0, 125.0, TRANSMIT_POWER_DBM, 8, true);

		/* Setup radio TX pin. */
		pinMode(dio2pin, OUTPUT);
		digitalWrite(dio2pin, LOW);
	}

	bool RadioCommander::init(ksf::ksComposable* owner)
	{
		mqttConnWp = owner->findComponent<ksf::comps::ksMqttConnector>();

		if (auto mqttConnSp = mqttConnWp.lock())
		{
			mqttConnSp->onConnected->registerEvent(connEventHandleSp, std::bind(&RadioCommander::onMqttConnected, this));
			mqttConnSp->onMesssage->registerEvent(msgEventHandleSp, std::bind(&RadioCommander::onMqttMessage, this, _1, _2));
		}
	
		return true;
	}

	void RadioCommander::onMqttConnected()
	{
		if (auto mqttConnSp = mqttConnWp.lock())
			mqttConnSp->subscribe(rfTopicPrefix + "#");
	}

	void RadioCommander::sendMqttInfo(const std::string& info)
	{
		if (auto mqttConnSp = mqttConnWp.lock())
			mqttConnSp->publish("log", info);
	}

	void RadioCommander::onMqttMessage(const std::string_view& topic, const std::string_view& payload)
	{
		if (topic.find(rfTopicPrefix) == std::string::npos)
			return;

		/* Radio commander payload should be one char ('1' or '0') */
		if (payload.length() == 1)
		{
			/* Check command queue overflow. */
			if (commandQueue.size() > 20)
			{
				/* If overflow, then blink RED wifi LED. */
				if (auto wifiLedSp = wifiLedWp.lock())
					if (!wifiLedSp->isBlinking())
						wifiLedSp->setBlinking(100, 5);

				/* ... and tell MQTT via log channel about that. */
				sendMqttInfo("RadioCmd: Queue is full - discarding!");
				return;
			}

			/*	
				Try split topic.

				Topics like OOK/1234/16 will be handled by nexa protocol.
				Topics like OOK/1 will be handled by ningbo protocol.
			 */
			auto delim_idx = topic.find('/', rfTopicPrefix.length() + 1);

			uint32_t address{0};
			int16_t unit{-1};
			
			if (delim_idx != std::string::npos)
			{
				address = std::stol(std::string(topic.substr(rfTopicPrefix.length(), delim_idx - rfTopicPrefix.length())));
				unit =  std::stol(std::string(topic.substr(delim_idx + 1)));
			}
			else
			{
				address = std::stol(std::string(topic.substr(rfTopicPrefix.length())));
			}

			/* If command queue is empty, enable transmitter and.. */
			if (commandQueue.empty())
				radioModule->transmitDirect();

			/* Push address/unit to command queue. */
			commandQueue.push({payload[0] == '1', address, unit, (uint8_t)(unit > 0 ? 6 : 9)});
		}
	}

	void RadioCommander::forceStandby()
	{
		/* Erase command queue. */
		commandQueue = {};

		/* Set randio module standby state. */
		if (radioModule)
			radioModule->standby();
	}

	void RadioCommander::handleRadioCommand(RadioCommand &command)
	{
		if (auto radioLedSp = radioLedWp.lock())
		{
			/* Here we decide if we use ningbo protocol or nexa protocol. */
			if (command.unit == -1)
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
			auto& currentCommand = commandQueue.front();
			handleRadioCommand(currentCommand);
			
			/* Check if it's last repeat. */
			if (currentCommand.repeats <= 0)
			{
				sendMqttInfo(
					"RadioCmd: Sent! "
					"[ A: " + std::to_string(currentCommand.address) + 
					" | U: " + std::to_string(currentCommand.unit) + 
					" | V: " + std::to_string(currentCommand.enable) + " ]"
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