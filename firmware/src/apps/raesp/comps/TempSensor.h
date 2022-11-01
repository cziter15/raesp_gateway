/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#pragma once

#include <ksIotFrameworkLib.h>

class DS18B20;

namespace apps::raesp::comps
{
	class TempSensor : public ksf::ksComponent, public std::enable_shared_from_this<TempSensor>
	{
		KSF_RTTI_DECLARATIONS(TempSensor, ksf::ksComponent)
		
		protected:
			std::weak_ptr<ksf::comps::ksMqttConnector> mqttConnWp;			//< Weak pointer to mqtt connector.
			std::shared_ptr<DS18B20> ds18handler;							//< Temp sensor handle ptr.

			ksf::ksSimpleTimer measurementTimer;

			uint8_t dataPin{std::numeric_limits<uint8_t>().max()};			//< Temp sensor data pin.
			uint8_t enabPin{std::numeric_limits<uint8_t>().max()};			//< Temp sensor emable pin.
			
			ksf::ksComposable* owner{nullptr};								//< Parent composable (app) pointer.

			void onMqttConnected();
			void onMqttMessage(const std::string_view& topic, const std::string_view& payload);
			void measureAndPublish(const std::shared_ptr<ksf::comps::ksMqttConnector>& mqttConnSp);

		public:
			TempSensor(uint8_t dataPin, uint8_t enabPin, uint32_t tempUpdateInterval);
			bool init(ksf::ksComposable* owner) override;
			bool loop() override;
	};
}