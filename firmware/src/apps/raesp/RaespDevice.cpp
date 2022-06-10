#include "RaespDevice.h"
#include "../../board.h"
#include "../config/RaespDeviceConfig.h"
#include "ArduinoOTA.h"

#include <RadioLib.h>
#include "RF/NexaTransmitter.h"
#include "RF/NingboTransmitter.h"

using namespace std::placeholders;

namespace raesp
{
	bool RaespDevice::init()
	{
		/* Create required components (Wifi and Mqtt debug). */
		addComponent<ksf::comps::ksWifiConnector>(config::RaespDeviceConfig::RaespDeviceName);
		addComponent<ksf::comps::ksMqttDebugResponder>();

		/* Create mqttConnector and statusLed components. */
		mqtt_wp = addComponent<ksf::comps::ksMqttConnector>();

		statusLed_wp = addComponent<ksf::comps::ksLed>(CFG_WIFI_LED);
		radioLed_wp = addComponent<ksf::comps::ksLed>(CFG_RADIO_LED);

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
			mqtt_sp->onMesssage->registerEvent(msgEventHandle_sp, std::bind(&RaespDevice::onMqttMessage, this, _1, _2));
			mqtt_sp->onConnected->registerEvent(connEventHandle_sp, std::bind(&RaespDevice::onMqttConnected, this));
			mqtt_sp->onDisconnected->registerEvent(disEventHandle_sp, std::bind(&RaespDevice::onMqttDisconnected, this));
		}

		/* Initialize Radio Module / radio backend. */
		RadioModule = std::make_shared<Module>(CFG_NSS_PIN, CFG_DIO0_PIN, CFG_RST_PIN, CFG_DIO2_PIN);
		RadioFrontend = std::make_shared<SX1278>(RadioModule.get());

		/* Setup OOK. */
		RadioFrontend->beginFSK(TRANSMIT_FREQ, 4.8, 5.0, 125.0, TRANSMIT_POWER_DBM, 8, true);

		/* Initialize protocols. */
		NexaRF = std::make_shared<NexaTransmitter>(CFG_DIO2_PIN, CFG_RADIO_LED);
		NingboRF = std::make_shared<NingboTransmitter>(CFG_DIO2_PIN, CFG_RADIO_LED);

		/* Start LED blinking on finished init. */
		if (auto statusLed_sp = statusLed_wp.lock())
			statusLed_sp->setBlinking(500);

		/* We want to stop RF before flash start. */
		ArduinoOTA.onStart([&]() {
			RadioFrontend->standby();
		});

		/* Application finished initialization, return true as it succedeed. */
		return true;
	}

	void RaespDevice::onMqttDisconnected()
	{
		if (auto statusLed_sp = statusLed_wp.lock())
			statusLed_sp->setBlinking(500);
	}

	void RaespDevice::onMqttConnected()
	{
		if (auto statusLed_sp = statusLed_wp.lock())
			statusLed_sp->setBlinking(0);

		if (auto mqtt_sp = mqtt_wp.lock())
			mqtt_sp->subscribe("#");
	}

	void RaespDevice::onMqttMessage(const String& topic, const String& payload)
	{
		if (payload.length() == 1)
		{
			auto delim_idx = topic.indexOf('/');
			bool bEnable = payload[0] == '1';

			if (auto mqtt_sp = mqtt_wp.lock())
			{
				RadioFrontend->transmitDirect();

				if (delim_idx >= 0)
				{
					uint32_t address = topic.substring(0, delim_idx).toInt();
					uint8_t unit = topic.substring(delim_idx + 1).toInt();

					NexaRF->setSwitch(bEnable, address, unit);
					mqtt_sp->publish("log", "[NexaRF] Unit: " + String(address) + ":" + String(unit) + "=" + payload);
				}
				else
				{
					NingboRF->setSwitch(topic.toInt(), bEnable);
					mqtt_sp->publish("log", "[NingboRF] Unit: " + topic + "=" + payload);
				}

				RadioFrontend->standby();
			}
		}
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