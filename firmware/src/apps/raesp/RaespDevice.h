#pragma once
#include <ksIotFrameworkLib.h>
#include "ArduinoOTA.h"

namespace raesp
{
	namespace comps
	{
		class RadioCommander;
	}

	class RaespDevice : public ksf::ksApplication
	{
		protected:
			ArduinoOTAClass ArduinoOTA;

			std::weak_ptr<ksf::comps::ksLed> wifiLedWp;

			std::weak_ptr<ksf::comps::ksMqttConnector> mqttConnWp;
			std::weak_ptr<comps::RadioCommander> radioCommanderWp;

			std::shared_ptr<ksf::evt::ksEventHandle> connEventHandleSp, disEventHandleSp;

			void onMqttMessage(const String& topic, const String& payload);
			void onMqttConnected();
			void onMqttDisconnected();

		public:
			bool init() override;
			bool loop() override;
	};
}