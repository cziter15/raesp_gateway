#include "RadioCommander.h"
#include <RadioLib.h>
#include "board.h"

#include "../protocols/nexa.h"
#include "../protocols/ningbo.h"

using namespace std::placeholders;

namespace raesp::comps
{
	RadioCommander::RadioCommander(	uint8_t ssPin, uint8_t dio0pin, uint8_t rstPin, uint8_t dio2pin, ksLedWP wifiLed, ksLedWP radioLed )
		: radioLed_wp(radioLed), wifiLed_wp(wifiLed)
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
		mqtt_wp = owner->findComponent<ksf::comps::ksMqttConnector>();

		if (auto mqtt_sp = mqtt_wp.lock())
		{
			mqtt_sp->onConnected->registerEvent(connEventHandle_sp, std::bind(&RadioCommander::onMqttConnected, this));
			mqtt_sp->onMesssage->registerEvent(msgEventHandle_sp, std::bind(&RadioCommander::onMqttMessage, this, _1, _2));
		}
	
		return true;
	}

	void RadioCommander::onMqttConnected()
	{
		if (auto mqtt_sp = mqtt_wp.lock())
			mqtt_sp->subscribe(rfTopicPrefix + "#");
	}

	void RadioCommander::sendMqttInfo(const String& info)
	{
		if (auto mqtt_sp = mqtt_wp.lock())
			mqtt_sp->publish("log", info);
	}

	void RadioCommander::onMqttMessage(const String& topic, const String& payload)
	{
		if (!topic.startsWith(rfTopicPrefix))
			return;

		/* Radio commander payload should be one char ('1' or '0') */
		if (payload.length() == 1)
		{
			/* Check command queue overflow. */
			if (commandQueue.size() > 20)
			{
				/* If overflow, then blink RED wifi LED. */
				if (auto wifiLed_sp = wifiLed_wp.lock())
					if (!wifiLed_sp->isBlinking())
						wifiLed_sp->setBlinking(100, 5);

				/* ... and tell MQTT via log channel about that. */
				sendMqttInfo("RadioCmd: Queue is full - discarding!");
				return;
			}

			/*	
				Try split topic.

				Topics like OOK/1234/16 will be handled by nexa protocol.
				Topics like OOK/1 will be handled by ningbo protocol.
			 */
			auto delim_idx = topic.indexOf('/', rfTopicPrefix.length());

			uint32_t address{0};
			int16_t unit{-1};
			
			if (delim_idx >= 0)
			{
				address = topic.substring(9, delim_idx).toInt();
				unit = topic.substring(delim_idx + 1).toInt();
			}
			else
			{
				address = topic.substring(rfTopicPrefix.length()).toInt();
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
		if (auto radioLed_sp = radioLed_wp.lock())
		{
			/* Here we decide if we use ningbo protocol or nexa protocol. */
			if (command.unit == -1)
				protocols::tx_ningbo_switch({radioPhy->getGpio(), radioLed_sp->getPin()}, command.enable, command.address);
			else
				protocols::tx_nexa_switch({radioPhy->getGpio(), radioLed_sp->getPin()}, command.enable, command.address, command.unit);
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
					"[ A: " + String(currentCommand.address) + 
					" | U: " + String(currentCommand.unit) + 
					" | V: " + String(currentCommand.enable) + " ]"
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