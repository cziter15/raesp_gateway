/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the RaespDevice (WiFi 433 MHz bridge) firmware.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#include "TempSensor.h"
#include <DS18B20.h>

using namespace std::placeholders;

namespace apps::raesp::comps
{
	#define TEMP_SENSOR_INVALID_TEMP 85.0f

	TempSensor::TempSensor(uint8_t dataPin, uint32_t tempUpdateInterval, std::optional<uint8_t> resolution, std::optional<uint8_t> enabPin)
		: dataPin(dataPin), enabPin(enabPin), resolution(resolution), measurementTimer(tempUpdateInterval)
	{}

	bool TempSensor::init(ksf::ksApplication* owner)
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
		auto cachedEnabGPC{enabPin.has_value() ? GPC(*enabPin) : 0};	// enabPin ctrl registry.
		auto cachedEnabGPF{enabPin.has_value() ? GPF(*enabPin) : 0};	// enabPin func registry.

		auto cachedDataGPC{GPC(dataPin)};	// dataPin ctrl registry.
		auto cachedDataGPF{GPF(dataPin)};	// dataPin func registry.

		auto cachedGPEC{GPEC};				// GPEC registry.

		/* Cache data pin state. */
		auto cachedEnabState{LOW};
		auto cachedDataState{digitalRead(dataPin)};

		/* Enable power for the sensor. */
		if (enabPin.has_value())
		{
			cachedEnabState = digitalRead(*enabPin);
			pinMode(*enabPin, OUTPUT);
			digitalWrite(*enabPin, HIGH);
		}

		/* Create DS18 handler. */
		if (!ds18handler)
		{
			ds18handler = std::make_shared<DS18B20>(dataPin);
			if (ds18handler && resolution.has_value()) 
				ds18handler->setResolution(*resolution);
		}

		/* Handle temperature measurement. */
		if (ds18handler && ds18handler->getNumberOfDevices() > 0)
		{
			auto tempC{ds18handler->getTempC()};
			if (fabsf(TEMP_SENSOR_INVALID_TEMP - tempC) > std::numeric_limits<float>::epsilon())
				mqttConnSp->publish(PGM_("tempSensor"), ksf::to_string(tempC, 1));
		}
		else
		{
			/* Queue ourselves for removal. */
			owner->markComponentToRemove(shared_from_this());
		}

		/* Disable power for the sensor. */
		if (enabPin.has_value())
			digitalWrite(*enabPin, cachedEnabState);

		/* Restore data pin state. */
		pinMode(dataPin, OUTPUT);
		digitalWrite(dataPin, cachedDataState);

		/* Restore saved pin configuration (pin mode). */
		if (enabPin.has_value())
		{
			GPC(*enabPin) = cachedEnabGPC;	// enabPin ctrl registry.
			GPF(*enabPin) = cachedEnabGPF;	// enabPin func registry.
		}

		GPC(dataPin) = cachedDataGPC;	// dataPin ctrl registry.
		GPF(dataPin) = cachedDataGPF;	// dataPin func registry.

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
