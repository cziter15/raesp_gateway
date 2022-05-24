#pragma once
#include <ksIotFrameworkLib.h>

class RaespDevice : public ksf::ksApplication
{
	protected:
		std::weak_ptr<ksf::comps::ksLed> radioLed_wp, statusLed_wp;		//< Weak pointer to Radio Led component.
		std::weak_ptr<ksf::comps::ksMqttConnector> mqtt_wp;
		std::shared_ptr<ksf::evt::ksEventHandle> connEventHandle_sp, msgEventHandle_sp, disEventHandle_sp;

		std::shared_ptr<class Module> RadioModule;
		std::shared_ptr<class SX1278> RadioFrontend;
		std::shared_ptr<class NexaTransmitter> NexaRF;
		std::shared_ptr<class NingboTransmitter> NingboRF;

		void onMqttMessage(const String& topic, const String& payload);
		void onMqttConnected();
		void onMqttDisconnected();

	public:
		bool init() override;
		bool loop() override;
};