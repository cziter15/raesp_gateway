#pragma once
#include <ksIotFrameworkLib.h>

namespace raesp
{
	namespace comps
	{
		class RadioCommander;
	}

	class RaespDevice : public ksf::ksApplication
	{
		protected:
			std::weak_ptr<ksf::comps::ksLed> wifiLed_wp, radioLed_wp;

			std::weak_ptr<ksf::comps::ksMqttConnector> mqtt_wp;
			std::weak_ptr<comps::RadioCommander> radioCommander_wp;

			std::shared_ptr<ksf::evt::ksEventHandle> connEventHandle_sp, disEventHandle_sp;

			void onMqttMessage(const String& topic, const String& payload);
			void onMqttConnected();
			void onMqttDisconnected();

		public:
			bool init() override;
			bool loop() override;
	};
}