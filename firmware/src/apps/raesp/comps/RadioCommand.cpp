#include "RadioCommand.h"
#include <RadioLib.h>
#include "board.h"

#include "../protocols/nexa.h"
#include "../protocols/ningbo.h"

using namespace std::placeholders;

namespace raesp::comps
{
	RadioCommander::RadioCommander(uint8_t ssPin, uint8_t dio0pin, uint8_t rstPin, uint8_t dio2pin, uint8_t statusPin) 
		: ledPin(statusPin)
	{
		radioModule = std::make_shared<Module>(ssPin, dio0pin, rstPin, dio2pin);
		radioFrontend = std::make_shared<SX1278>(radioModule.get());

		radioFrontend->beginFSK(TRANSMIT_FREQ, 4.8, 5.0, 125.0, TRANSMIT_POWER_DBM, 8, true);

		pinMode(dio2pin, OUTPUT);
		pinMode(ledPin, OUTPUT);

		digitalWrite(dio2pin, LOW);
		digitalWrite(ledPin, LOW);
	}

	bool RadioCommander::init(ksf::ksComposable* owner)
	{
		mqtt_wp = owner->findComponent<ksf::comps::ksMqttConnector>();
		wifiLed_wp = owner->findComponent<ksf::comps::ksLed>();

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
			mqtt_sp->subscribe("#");
	}

	void RadioCommander::onMqttMessage(const String& topic, const String& payload)
	{
		if (payload.length() == 1)
		{
			if (commandQueue.size() > 20)
			{
				if (auto wifiLed_sp = wifiLed_wp.lock())
					if (!wifiLed_sp->isBlinking())
						wifiLed_sp->setBlinking(100, 5);

				return;
			}

			auto delim_idx = topic.indexOf('/');

			uint32_t address{0};
			int16_t unit{-1};
			
			if (delim_idx >= 0)
			{
				address = topic.substring(0, delim_idx).toInt();
				unit = topic.substring(delim_idx + 1).toInt();
			}
			else
			{
				address = topic.toInt();
			}

			if (commandQueue.empty())
				radioFrontend->transmitDirect();

			commandQueue.push({payload[0] == '1', address, unit, (uint8_t)(unit > 0 ? 9 : 6)});
		}
	}

	void RadioCommander::forceStandby()
	{
		commandQueue = {};

		if (radioFrontend)
			radioFrontend->standby();
	}

	void RadioCommander::handleRadioCommand(const RadioCommand &command)
	{
		noInterrupts();

		if (command.unit == -1)
			protocols::ningbo_transmit({radioModule->getGpio(), ledPin}, command.enable, command.address);
		else
			protocols::nexa_transmit({radioModule->getGpio(), ledPin}, command.enable, command.address, command.unit);

		interrupts();
	}

	bool RadioCommander::loop()
	{
		if (!commandQueue.empty())
		{
			auto& nextCmd = commandQueue.front();
			handleRadioCommand(nextCmd);
			
			if (--nextCmd.retries <= 0)
			{
				if (auto mqtt_sp = mqtt_wp.lock())
				{
					mqtt_wp.lock()->publish("log", 
						"Radio command [A: " + String(nextCmd.address) + 
						", U: " + String(nextCmd.unit) + 
						"], with value: " + String(nextCmd.enable)
					);
				}

				commandQueue.pop();
				
				if (commandQueue.empty())
					radioFrontend->standby();
			}
		}

		return true;
	}
}