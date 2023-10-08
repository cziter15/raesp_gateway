/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the RaespDevice (WiFi 433 MHz bridge) firmware.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */
 
#pragma once

#include <ksIotFrameworkLib.h>
#include <optional>

class DS18B20;

namespace apps::raesp::comps
{
	class TempSensor : public ksf::ksComponent, public std::enable_shared_from_this<TempSensor>
	{
		KSF_RTTI_DECLARATIONS(TempSensor, ksf::ksComponent)
		
		protected:
			std::weak_ptr<ksf::comps::ksMqttConnector> mqttConnWp;			// Weak pointer to mqtt connector.
			std::unique_ptr<DS18B20> ds18handler;							// Temp sensor handle ptr.

			uint8_t dataPin{0};												// Temp sensor data pin.
			std::optional<uint8_t> enabPin;									// Temp sensor emable pin.
			std::optional<uint8_t> resolution;								// Requested resolution.

			ksf::ksSimpleTimer measurementTimer;							// Measurement timer (to measure temp at specified interval).

			/*
				Event handler method called when MQTT service connected to server. 
			*/
			void onMqttConnected();

			/*
				Event handler method called when MQTT service receives a message.

				@param topic Reference of topic string_view.
				@param message Reference of message string_view.
			*/
			void onMqttMessage(const std::string_view& topic, const std::string_view& payload);

			/*
				Retrieves data from temperature sensor (blocking).

				@param mqttConnSp Reference to shared pointer of MQTT component.
			*/
			void measureAndPublish(const std::shared_ptr<ksf::comps::ksMqttConnector>& mqttConnSp);

		public:
			/*
				Constructs TempSensor.

				@param dataPin OneWire data pin number.
				@param tempUpdateInterval Interval of measurements.
				@param resolution Optional resolution.
				@param enabPin Optional enabPin number.
			*/
			TempSensor(
				uint8_t dataPin, 
				uint32_t tempUpdateInterval = 300000UL, 
				std::optional<uint8_t> resolution = std::nullopt, 
				std::optional<uint8_t> enabPin = std::nullopt
			);

			/*
				Initializes TempSensor component.

				@param owner Pointer to owning application.
				@return True on success, false on fail.
			*/
			bool init(ksf::ksApplication* app) override;

			/*
				Handles TempSensor logic.
				@param app Pointer to owning application.
				@return True on success, false on fail.
			*/
			bool loop(ksf::ksApplication* app) override;

			/*
				Destructor to free up resources.
			*/
			virtual ~TempSensor();
	};
}