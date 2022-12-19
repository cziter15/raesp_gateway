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
#include <queue>

class Module;
class SX1278;

namespace apps::raesp::comps
{
	struct RadioCommand
	{
		bool enable;
		uint32_t address;
		int16_t unit;
		uint8_t repeats;
	};

	class RadioCommander : public ksf::ksComponent
	{
		KSF_RTTI_DECLARATIONS(RadioCommander, ksf::ksComponent)
		
		using ksLedWP = std::weak_ptr<ksf::comps::ksLed>;

		protected:

			static constexpr uint16_t MAX_TX_QUEUE_SIZE{20};									// Maximum queue size for RC requests.
			static constexpr int16_t RC_UNIT_NONE{-1};											// Value indicating no unit presence.

			std::shared_ptr<Module> radioPhy;													// Radio Phy module ptr.
			std::shared_ptr<SX1278> radioModule;												// Radio Module ptr.
			std::queue<RadioCommand> commandQueue;												// Radio commadn queue.

			std::weak_ptr<ksf::comps::ksMqttConnector> mqttConnWp;								// Weak pointer to mqtt connector.
			std::weak_ptr<ksf::comps::ksLed> radioLedWp, wifiLedWp;								// Weak pointers to LEDs.

			const std::string rfTopicPrefix{"rfswitch/"};										// RF command topic prefix.
			std::shared_ptr<ksf::evt::ksEventHandle> connEventHandleSp, msgEventHandleSp;		// Shared ptrs to events.

			/*
				Event handler method called when MQTT service receives a message.

				@param topic Topic reference (string_view).
				@param message Message (aka payload) reference (string_view).
			*/
			void onMqttMessage(const std::string_view& topic, const std::string_view& payload);

			/*
				Event handler method called when MQTT service connected to server. 
			*/
			void onMqttConnected();

			/*
				Wrapper function for publishin device log to specific MQTT topic.

				@param info Message to publish.
			*/
			void sendMqttInfo(const std::string& info) const;

			/*
				This funciton will handle one repeat of RadioCommand and will block to drive data pin.

				@param cmd Radio command reference.
			*/
			void IRAM_ATTR processRadioCommand(RadioCommand& cmd);
			
		public:
			/*
				Constructs RadioCommander.

				@param ssPin Radio Module SS pin number.
				@param dio0pin Radio Module DIO0 pin number.
				@param rstPin Radio Module reset pin number.
				@param dio2pin Radio Module DIO2 pin number.
				@param wifiLedWp Wifi LED component weak pointer.
				@param radioLedWp Radio LED component weak pointer.
			*/
			RadioCommander(uint8_t ssPin, uint8_t dio0pin, uint8_t rstPin, uint8_t dio2pin,	ksLedWP wifiLedWp, ksLedWP radioLedWp);

			/*
				Initializes RadioCommander component.

				@param owner Pointer to owning ksApplication.
				@return True on success, false on fail.
			*/
			bool init(ksf::ksApplication* owner) override;

			/* 
				Handles RadioCommander logic.

				@return True on success, false on fail.
			*/
			bool loop() override;

			/* 
				Forces standby state on Radio Module. Will clear radio command queue.
			*/
			void forceStandby();
	};
}