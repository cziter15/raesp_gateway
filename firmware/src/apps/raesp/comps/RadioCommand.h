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
			std::shared_ptr<Module> radioPhy;
			std::shared_ptr<SX1278> radioModule;

			std::weak_ptr<ksf::comps::ksMqttConnector> mqtt_wp;
			std::weak_ptr<ksf::comps::ksLed> wifiLed_wp, radioLed_wp;

			std::shared_ptr<ksf::evt::ksEventHandle> connEventHandle_sp, msgEventHandle_sp;

			std::queue<RadioCommand> commandQueue;

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