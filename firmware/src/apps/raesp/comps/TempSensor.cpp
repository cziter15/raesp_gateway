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

	bool TempSensor::init(ksf::ksApplication* app)
	{
		mqttConnWp = app->findComponent<ksf::comps::ksMqttConnector>();
		return true;
	}

	void TempSensor::measureAndPublish(const std::shared_ptr<ksf::comps::ksMqttConnector>& mqttConnSp)
	{
		/* 
			Skip measurement if LED pin (enabPin) is initially HIGH.
			When the LED is active (HIGH), it means other components are using it,
			so we shouldn't interfere. Use GPIO registers to check pin state.
			Verify pin is configured as OUTPUT before checking state.
		*/
		if (enabPin.has_value())
		{
			uint8_t pin{*enabPin};
			uint32_t pinMask{1U << pin};
			bool isOutput{(GPIO_REG_READ(GPIO_ENABLE_ADDRESS) & pinMask) != 0};
			if (isOutput && (GPIO_REG_READ(GPIO_IN_ADDRESS) & pinMask))
				return;
		}
		
		/* Cache GPIO registers for restoration. */
		auto cachedEnabGPC{enabPin.has_value() ? GPC(*enabPin) : 0};	// enabPin ctrl registry.
		auto cachedEnabGPF{enabPin.has_value() ? GPF(*enabPin) : 0};	// enabPin func registry.
		auto cachedDataGPC{GPC(dataPin)};								// dataPin ctrl registry.
		auto cachedDataGPF{GPF(dataPin)};								// dataPin func registry.
		auto cachedGPEC{GPEC};											// GPEC registry.
		
		/* Enable power for the sensor using GPIO registers. */
		if (enabPin.has_value())
		{
			uint8_t pin{*enabPin};
			uint32_t pinMask{1U << pin};

			GPF(pin) = GPFFS(GPFFS_GPIO(pin));							// Configure pin as GPIO (function 0)
			GPIO_REG_WRITE(GPIO_ENABLE_W1TS_ADDRESS, pinMask);			// Set pin as output
			GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, pinMask);				// Set pin HIGH
		}
		
		/* Create DS18 handler. */
		if (!ds18handler)
		{
			ds18handler = std::make_unique<DS18B20>(dataPin);
			if (ds18handler && resolution.has_value()) 
				ds18handler->setResolution(*resolution);
		}
		
		/* Handle temperature measurement. */
		if (ds18handler && ds18handler->getNumberOfDevices() > 0)
		{
			auto tempC{ds18handler->getTempC()};
			if (fabsf(TEMP_SENSOR_INVALID_TEMP - tempC) > std::numeric_limits<float>::epsilon())
				mqttConnSp->publish(PSTR("tempSensor"), ksf::to_string(tempC, 1));
		}
		else
		{
			/* Queue ourselves for removal. */
			componentState = ksf::ksComponentState::ToRemove;
		}
		
		/* Disable power for the sensor using GPIO registers. */
		if (enabPin.has_value())
		{
			uint32_t pinMask{1U << *enabPin};
			GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, pinMask);
		}
		
		/* Restore saved pin configuration (pin mode). */
		if (enabPin.has_value())
		{
			GPC(*enabPin) = cachedEnabGPC;
			GPF(*enabPin) = cachedEnabGPF;
		}
		GPC(dataPin) = cachedDataGPC;
		GPF(dataPin) = cachedDataGPF;
		GPEC = cachedGPEC;
	}

	bool TempSensor::loop(ksf::ksApplication* app)
	{
		if (measurementTimer.triggered())
			if (auto mqttConnSp{mqttConnWp.lock()})
				if (mqttConnSp->isConnected())
					measureAndPublish(mqttConnSp);

		return true;
	}

	TempSensor::~TempSensor() = default;
}
