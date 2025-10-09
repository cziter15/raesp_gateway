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
	#define PARASITIC_CAPACITOR_CHARGE_DELAY_MS 2

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
		/* Check if any pin that will be used is currently HIGH (LED active). */
		if (digitalRead(dataPin) == HIGH)
			return;	// Skip measurement to avoid conflict with LED.
		
		if (enabPin.has_value() && digitalRead(*enabPin) == HIGH)
			return;	// Skip measurement to avoid conflict with LED.

		/* Enable power for the sensor. */
		if (enabPin.has_value())
		{
			pinMode(*enabPin, OUTPUT);
			digitalWrite(*enabPin, HIGH);
			
			/* Delay to allow parasitic capacitor to charge (per DS18B20 specs). */
			delay(PARASITIC_CAPACITOR_CHARGE_DELAY_MS);
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

		/* Disable power for the sensor and set pins to LOW. */
		if (enabPin.has_value())
		{
			pinMode(*enabPin, OUTPUT);
			digitalWrite(*enabPin, LOW);
		}

		/* Set data pin to LOW. */
		pinMode(dataPin, OUTPUT);
		digitalWrite(dataPin, LOW);
	}

	bool TempSensor::loop(ksf::ksApplication* app)
	{
		if (measurementTimer.triggered())
			if (auto mqttConnSp{mqttConnWp.lock()})
				measureAndPublish(mqttConnSp);

		return true;
	}

	TempSensor::~TempSensor() = default;
}
