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
		int unit;
		uint8_t retries;
	};

	class RadioCommander : public ksf::ksComponent
	{
		protected:
			std::shared_ptr<Module> radioModule;
			std::shared_ptr<SX1278> radioFrontend;

			std::weak_ptr<ksf::comps::ksMqttConnector> mqtt_wp;
			std::weak_ptr<ksf::comps::ksLed> wifiLed_wp;

			std::shared_ptr<ksf::evt::ksEventHandle> connEventHandle_sp, msgEventHandle_sp;

			uint8_t ledPin;

			std::queue<RadioCommand> commandQueue;

			void onMqttMessage(const String& topic, const String& payload);
			void onMqttConnected();

			void IRAM_ATTR handleRadioCommand(const RadioCommand& cmd);
			
		public:
			RadioCommander(uint8_t ssPin, uint8_t dio0pin, uint8_t rstPin, uint8_t dio2pin, uint8_t statusPin);
			bool init(ksf::ksComposable* owner) override;
			bool loop() override;
			void forceStandby();
	};
}