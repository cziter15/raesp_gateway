/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the RaespDevice (WiFi 433 MHz bridge) firmware.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#include "RaespDeviceConfig.h"
#include "board.h"

namespace apps::config
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
