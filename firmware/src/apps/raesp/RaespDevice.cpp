/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#include "RaespDevice.h"
#include "board.h"
#include "../config/RaespDeviceConfig.h"
#include "comps/RadioCommander.h"

using namespace std::placeholders;

namespace apps::raesp
{
	bool RaespDevice::init()
	{
		/* Create required components (Wifi and Mqtt debug). */
		addComponent<ksf::comps::ksWifiConnector>(config::RaespDeviceConfig::RaespDeviceName);
		addComponent<ksf::comps::ksMqttDebugResponder>();

		/* Create mqttConnector component. */
		mqttConnWp = addComponent<ksf::comps::ksMqttConnector>();

		/* Create LED components. */
		wifiLedWp = addComponent<ksf::comps::ksLed>(CFG_WIFI_LED);
		auto radioLedWp{addComponent<ksf::comps::ksLed>(CFG_RADIO_LED)};

		/* Create RadioCommander component. */
		radioCommanderWp = addComponent<comps::RadioCommander>(CFG_NSS_PIN, CFG_DIO0_PIN, CFG_RST_PIN, CFG_DIO2_PIN, wifiLedWp, radioLedWp);

		/* Try to initialize superclass. It will initialize our components and tcpip (due to WiFi component). */
		if (!ksApplication::init())
			return false;

		/* ArduinoOTA is allowed to start AFTER app initialization, because tcpip must be initialized. */
		ArduinoOTA.setHostname(config::RaespDeviceConfig::RaespDeviceName);
		ArduinoOTA.setPassword("ota_ksiotframework");
		ArduinoOTA.begin();
		
		/* Bind to MQTT callbacks. */
		if (auto mqttConnSp{mqttConnWp.lock()})
		{
			mqttConnSp->onConnected->registerEvent(connEventHandleSp, std::bind(&RaespDevice::onMqttConnected, this));
			mqttConnSp->onDisconnected->registerEvent(disEventHandleSp, std::bind(&RaespDevice::onMqttDisconnected, this));
		}

		/* Start LED blinking on finished init. */
		if (auto statusLed_sp{wifiLedWp.lock()})
			statusLed_sp->setBlinking(500);

		/* We want to stop RF before flash start. */
		ArduinoOTA.onStart([&]() {
			if (auto RadioFrontend{radioCommanderWp.lock()})
				RadioFrontend->forceStandby();
		});

		/* Application finished initialization, return true as it succedeed. */
		return true;
	}

	void RaespDevice::onMqttDisconnected()
	{
		if (auto wifiLedSp{wifiLedWp.lock()})
			wifiLedSp->setBlinking(500);
	}

	void RaespDevice::onMqttConnected()
	{
		if (auto wifiLedSp{wifiLedWp.lock()})
			wifiLedSp->setBlinking(0);
	}

	bool RaespDevice::loop()
	{
		/* Handle OTA stuff. */
		ArduinoOTA.handle();

		/*
			Return to superclass application loop.
			It handles all our components and whole app logic.
		*/
		return ksApplication::loop();
	}
}