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
#include "ArduinoOTA.h"

namespace apps::raesp
{
	namespace comps
	{
		class RadioCommander;
	}

	class RaespDevice : public ksf::ksApplication
	{
		protected:

			std::weak_ptr<ksf::comps::ksLed> wifiLedWp;										// Weeak pointer to WiFi status LED component.
			std::weak_ptr<ksf::comps::ksMqttConnector> mqttConnWp;							// Weeak pointer to MQTT component.
			std::weak_ptr<comps::RadioCommander> radioCommanderWp;							// Weeak pointer to RadioCommander component.

			std::unique_ptr<ksf::evt::ksEventHandle> connEventHandleSp, disEventHandleSp;	// Handles to MQTT events.
			std::unique_ptr<ksf::evt::ksEventHandle> otaUpdateStartEventHandleSp;			// Handles to OTA events.

			/*
				Event handler method called when MQTT service connected to server. 
			*/
			void onMqttConnected();

			/*
				Event handler method called when MQTT service disconnected. 
			*/
			void onMqttDisconnected();

			/*
				Event handler method called when OTA update starts. 
			*/
			void onOtaUpdateStart();

		public:
			/* 
				Initializes RaespDevice application.

				@return True on success, false on fail.
			*/
			bool init() override;

			/* 
				Handles RaespDevice application logic.

				@return True on success, false on fail.
			*/
			bool loop() override;
	};
}