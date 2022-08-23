#pragma once
#include <ksIotFrameworkLib.h>
#include <queue>

class Module;
class SX1278;

namespace raesp::comps
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
		using ksLedWP = std::weak_ptr<ksf::comps::ksLed>;
		
		protected:
			std::shared_ptr<Module> radioPhy;													//< Radio Phy module ptr.
			std::shared_ptr<SX1278> radioModule;												//< Radio Module ptr.

			std::weak_ptr<ksf::comps::ksMqttConnector> mqttConnWp;								//< Weak pointer to mqtt connector.
			std::weak_ptr<ksf::comps::ksLed> radioLedWp, wifiLedWp;								//< Weak pointers to LEDs.

			std::shared_ptr<ksf::evt::ksEventHandle> connEventHandleSp, msgEventHandleSp;		//< Shared ptrs to events.

			std::queue<RadioCommand> commandQueue;												//< Radio commadn queue.

			const std::string rfTopicPrefix{"rfswitch/"};										//< RF command topic prefix.

			void onMqttMessage(const std::string_view& topic, const std::string_view& payload);
			void onMqttConnected();
			void sendMqttInfo(const std::string& info);

			void IRAM_ATTR handleRadioCommand(RadioCommand& cmd);
			
		public:
			RadioCommander(uint8_t ssPin, uint8_t dio0pin, uint8_t rstPin, uint8_t dio2pin,	ksLedWP wifiLedWp, ksLedWP radioLedWp);
			bool init(ksf::ksComposable* owner) override;
			bool loop() override;
			void forceStandby();
	};
}