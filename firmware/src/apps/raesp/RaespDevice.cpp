/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the RaespDevice (WiFi 433 MHz bridge) firmware.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#include "RaespDevice.h"
#include "board.h"
#include "../config/RaespDeviceConfig.h"
#include "comps/RadioCommander.h"

#if USE_TEMP_SENSOR
	#include "comps/TempSensor.h"
#endif

using namespace std::placeholders;

namespace apps::raesp
{
	bool RaespDevice::init()
	{
		/* Create required components (Wifi and Mqtt debug). */
		addComponent<ksf::comps::ksWifiConnector>(config::RaespDeviceConfig::RaespDeviceName);
		addComponent<ksf::comps::ksDevStatMqttReporter>();

		/* Create OTA component. */
		auto otaUpdaterWp{addComponent<ksf::comps::ksDevicePortal>()};

		/* Create mqttConnector component. */
		mqttConnWp = addComponent<ksf::comps::ksMqttConnector>();

		/* Create LED components. */
		wifiLedWp = addComponent<ksf::comps::ksLed>(CFG_STATUS_LED);
		auto radioLedWp{addComponent<ksf::comps::ksLed>(CFG_RADIO_LED)};

		/* Create Temperature sensing component. */
		#if USE_TEMP_SENSOR
			addComponent<comps::TempSensor>(TEMP_DATA_PIN, TEMP_UPDATE_INTERVAL_MS, TEMP_SENSOR_RESOLUTION, TEMP_ENAB_PIN);
		#endif

		/* Create RadioCommander component. */
		radioCommanderWp = addComponent<comps::RadioCommander>(CFG_NSS_PIN, CFG_DIO0_PIN, CFG_RST_PIN, CFG_DIO2_PIN, wifiLedWp, radioLedWp);

		/* Bind to MQTT callbacks. */
		if (auto mqttConnSp{mqttConnWp.lock()})
		{
			mqttConnSp->onConnected->registerEvent(connEventHandleSp, std::bind(&RaespDevice::onMqttConnected, this));
			mqttConnSp->onDisconnected->registerEvent(disEventHandleSp, std::bind(&RaespDevice::onMqttDisconnected, this));
		}

		/* Bind to OTA callbacks. */
		if (auto otaUpdaterSp{otaUpdaterWp.lock()})
			otaUpdaterSp->onUpdateStart->registerEvent(otaUpdateStartEventHandleSp, std::bind(&RaespDevice::onOtaUpdateStart, this));

		/* Start LED blinking on finished init. */
		if (auto wifiLedSp{wifiLedWp.lock()})
			wifiLedSp->setBlinking(500);

		/* Disable GPIO2 for now (LED) */
		pinMode(2, OUTPUT);
		digitalWrite(2, HIGH);
		
		/* Application finished initialization, return true as it succedeed. */
		return true;
	}

	void RaespDevice::onOtaUpdateStart()
	{
		if (auto radioCommanderSp{radioCommanderWp.lock()})
			radioCommanderSp->forceStandby();
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
		/*
			Return to superclass application loop.
			It handles all our components and whole app logic.
		*/
		return ksApplication::loop();
	}
}
