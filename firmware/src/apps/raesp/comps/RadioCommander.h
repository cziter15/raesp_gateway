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

			std::weak_ptr<ksf::comps::ksMqttConnector> mqtt_wp;									//< Weak pointer to mqtt connector.
			std::weak_ptr<ksf::comps::ksLed> radioLed_wp, wifiLed_wp;							//< Weak pointers to LEDs.

			std::shared_ptr<ksf::evt::ksEventHandle> connEventHandle_sp, msgEventHandle_sp;		//< Shared ptrs to events.

			std::queue<RadioCommand> commandQueue;												//< Radio commadn queue.

			const String rfTopicPrefix{"rfswitch/"};											//< RF command topic prefix.

			void onMqttMessage(const String& topic, const String& payload);
			void onMqttConnected();
			void sendMqttInfo(const String& info);

			void IRAM_ATTR handleRadioCommand(RadioCommand& cmd);
			
		public:
			RadioCommander(uint8_t ssPin, uint8_t dio0pin, uint8_t rstPin, uint8_t dio2pin,	ksLedWP wifiLed, ksLedWP radioLed);
			bool init(ksf::ksComposable* owner) override;
			bool loop() override;
			void forceStandby();
	};
}