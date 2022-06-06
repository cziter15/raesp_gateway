#include "RaespDeviceConfig.h"
#include "../../board.h"

namespace config
{
	const char RaespDeviceConfig::RaespDeviceName[] = "Raesp";

	bool RaespDeviceConfig::init()
	{
		addComponent<ksf::comps::ksWiFiConfigurator>(RaespDeviceName);
		addComponent<ksf::comps::ksMqttConfigProvider>();

		addComponent<ksf::comps::ksLed>(CFG_WIFI_LED);
		addComponent<ksf::comps::ksLed>(CFG_RADIO_LED);

		return ksApplication::init();
	}

	bool RaespDeviceConfig::loop()
	{
		return ksApplication::loop();
	}
}
