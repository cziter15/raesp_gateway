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

			static constexpr uin16_t MAX_TX_QUEUE_SIZE{20};										// Maximum queue size for RC requests.
			static constexpr int16_t RC_UNIT_NONE{-1};											// Value indicating no unit presence.

			std::shared_ptr<Module> radioPhy;													// Radio Phy module ptr.
			std::shared_ptr<SX1278> radioModule;												// Radio Module ptr.
			std::queue<RadioCommand> commandQueue;												// Radio commadn queue.

			std::weak_ptr<ksf::comps::ksMqttConnector> mqttConnWp;								// Weak pointer to mqtt connector.
			std::weak_ptr<ksf::comps::ksLed> radioLedWp, wifiLedWp;								// Weak pointers to LEDs.

			const std::string rfTopicPrefix{"rfswitch/"};										// RF command topic prefix.
			std::shared_ptr<ksf::evt::ksEventHandle> connEventHandleSp, msgEventHandleSp;		// Shared ptrs to events.

			void onMqttMessage(const std::string_view& topic, const std::string_view& payload);
			void onMqttConnected();
			void sendMqttInfo(const std::string& info) const;

			void IRAM_ATTR processRadioCommand(RadioCommand& cmd);
			
		public:
			RadioCommander(uint8_t ssPin, uint8_t dio0pin, uint8_t rstPin, uint8_t dio2pin,	ksLedWP wifiLedWp, ksLedWP radioLedWp);
			bool init(ksf::ksComposable* owner) override;
			bool loop() override;
			void forceStandby();
	};
}