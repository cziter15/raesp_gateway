/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#include "TempSensor.h"
#include <DS18B20.h>

using namespace std::placeholders;

namespace apps::raesp::comps
{
	TempSensor::TempSensor(uint8_t dataPin, uint8_t enabPin, uint32_t updateInterval, uint8_t resolution)
		: measurementTimer(updateInterval), dataPin(dataPin), enabPin(enabPin), resolution(resolution)
	{}

	bool TempSensor::init(ksf::ksComposable* owner)
	{
		this->owner = owner;
		mqttConnWp = owner->findComponent<ksf::comps::ksMqttConnector>();
		return true;
	}

	void TempSensor::measureAndPublish(const std::shared_ptr<ksf::comps::ksMqttConnector>& mqttConnSp)
	{
		/* 
			Well this is a bit tricky. Because we are sharing pins with LEDs, we need to cache Direction and LO/HI state.
			Then, when we are done, we should restore pin state and modes so it will look it didn't happen from other components.
		*/
		auto cachedEnabGPC{GPC(enabPin)};	// enabPin ctrl registry.
		auto cachedDataGPC{GPC(dataPin)};	// dataPin ctrl registry.
		auto cachedEnabGPF{GPF(enabPin)};	// enabPin func registry.
		auto cachedDataGPF{GPF(dataPin)};	// dataPin func registry.
		auto cachedGPEC{GPEC};				// GPEC registry.

		/* Cache data pin state. */
		auto cachedEnabState{LOW};
		auto cachedDataState{digitalRead(dataPin)};

		/* Enable power for the sensor. */
		if (enabPin != std::numeric_limits<uint8_t>().max())
		{
			cachedEnabState = digitalRead(enabPin);
			pinMode(enabPin, OUTPUT);
			digitalWrite(enabPin, HIGH);
		}

		/* Create DS18 handler. */
		if (!ds18handler)
		{
			ds18handler = std::make_shared<DS18B20>(dataPin);
			if (ds18handler && resolution > 0) 
				ds18handler->setResolution(resolution);
		}

		/* Handle temperature measurement. */
		if (ds18handler && ds18handler->getNumberOfDevices() > 0)
			mqttConnSp->publish("room_temp", ksf::to_string(ds18handler->getTempC(), 2));
		else
			owner->queueRemoveComponent(shared_from_this());

		/* Disable power for the sensor. */
		if (enabPin != std::numeric_limits<uint8_t>().max())
			digitalWrite(enabPin, cachedEnabState);

		/* Restore data pin state. */
		pinMode(dataPin, OUTPUT);
		digitalWrite(dataPin, cachedDataState);

		/* Restore saved pin configuration (pin mode). */
		GPC(dataPin) = cachedDataGPC;	// dataPin ctrl registry.
		GPC(enabPin) = cachedEnabGPC;	// enabPin ctrl registry.
		GPF(dataPin) = cachedDataGPF;	// dataPin func registry.
		GPF(enabPin) = cachedEnabGPF;	// enabPin func registry.
		GPEC = cachedGPEC;				// GPEC registry.
	}

	bool TempSensor::loop()
	{
		if (measurementTimer.triggered())
			if (auto mqttConnSp{mqttConnWp.lock()})
				measureAndPublish(mqttConnSp);

		return true;
	}
}
