#include "RaespDevice.h"
#include "board.h"
#include "../config/RaespDeviceConfig.h"
#include "comps/RadioCommand.h"
#include "ArduinoOTA.h"

using namespace std::placeholders;

namespace raesp
{
	bool RaespDevice::init()
	{
		/* Create required components (Wifi and Mqtt debug). */
		addComponent<ksf::comps::ksWifiConnector>(config::RaespDeviceConfig::RaespDeviceName);
		addComponent<ksf::comps::ksMqttDebugResponder>();

		/* Create mqttConnector component. */
		mqtt_wp = addComponent<ksf::comps::ksMqttConnector>();

		/* Create LED components. */
		wifiLed_wp = addComponent<ksf::comps::ksLed>(CFG_WIFI_LED);
		radioLed_wp = addComponent<ksf::comps::ksLed>(CFG_RADIO_LED);

		/* Create RadioCommander component. */
		radioCommander_wp = addComponent<comps::RadioCommander>(CFG_NSS_PIN, CFG_DIO0_PIN, CFG_RST_PIN, CFG_DIO2_PIN, wifiLed_wp, radioLed_wp);

		/* Try to initialize superclass. It will initialize our components and tcpip (due to WiFi component). */
		if (!ksApplication::init())
			return false;

		/* ArduinoOTA is allowed to start AFTER app initialization, because tcpip must be initialized. */
		ArduinoOTA.setHostname(config::RaespDeviceConfig::RaespDeviceName);
		ArduinoOTA.setPassword("ota_ksiotframework");
		ArduinoOTA.begin();
		
		/* Bind to MQTT callbacks. */
		if (auto mqtt_sp = mqtt_wp.lock())
		{
			mqtt_sp->onConnected->registerEvent(connEventHandle_sp, std::bind(&RaespDevice::onMqttConnected, this));
			mqtt_sp->onDisconnected->registerEvent(disEventHandle_sp, std::bind(&RaespDevice::onMqttDisconnected, this));
		}

		/* Start LED blinking on finished init. */
		if (auto statusLed_sp = wifiLed_wp.lock())
			statusLed_sp->setBlinking(500);

		/* We want to stop RF before flash start. */
		ArduinoOTA.onStart([&]() {
			if (auto RadioFrontend = radioCommander_wp.lock())
				RadioFrontend->forceStandby();
		});

		/* Application finished initialization, return true as it succedeed. */
		return true;
	}

	void RaespDevice::onMqttDisconnected()
	{
		if (auto statusLed_sp = wifiLed_wp.lock())
			statusLed_sp->setBlinking(500);
	}

	void RaespDevice::onMqttConnected()
	{
		if (auto statusLed_sp = wifiLed_wp.lock())
			statusLed_sp->setBlinking(0);
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