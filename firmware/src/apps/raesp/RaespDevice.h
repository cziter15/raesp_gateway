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

			std::shared_ptr<ksf::evt::ksEventHandle> connEventHandleSp, disEventHandleSp;	// Handles to MQTT events.

			/*
				Event handler method called when MQTT service receives a message.

				@param topic Reference of topic string_view.
				@param message Reference of message string_view.
			*/
			void onMqttMessage(const String& topic, const String& payload);

			/*
				Event handler method called when MQTT service connected to server. 
			*/
			void onMqttConnected();

			/*
				Event handler method called when MQTT service disconnected. 
			*/
			void onMqttDisconnected();

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