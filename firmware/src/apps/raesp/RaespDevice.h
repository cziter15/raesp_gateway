#pragma once
#include <ksIotFrameworkLib.h>

class Module;
class SX1278;
class NexaTransmitter;
class NingboTransmitter;

namespace raesp
{
	class RaespDevice : public ksf::ksApplication
	{
		protected:
			std::weak_ptr<ksf::comps::ksLed> radioLed_wp, statusLed_wp;		//< Weak pointer to Radio Led component.
			std::weak_ptr<ksf::comps::ksMqttConnector> mqtt_wp;
			std::shared_ptr<ksf::evt::ksEventHandle> connEventHandle_sp, msgEventHandle_sp, disEventHandle_sp;

			std::shared_ptr<Module> RadioModule;
			std::shared_ptr<SX1278> RadioFrontend;
			std::shared_ptr<NexaTransmitter> NexaRF;
			std::shared_ptr<NingboTransmitter> NingboRF;

			void onMqttMessage(const String& topic, const String& payload);
			void onMqttConnected();
			void onMqttDisconnected();

		public:
			bool init() override;
			bool loop() override;
	};
}