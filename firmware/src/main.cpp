/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the RaespDevice (WiFi 433 MHz bridge) firmware.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#include "apps/raesp/RaespDevice.h"
#include "apps/config/RaespDeviceConfig.h"

using namespace apps;

// the setup function runs once when you press reset or power the board
void setup()
{
	KSF_FRAMEWORK_INIT()
}

// the loop function runs over and over again until power down or reset
void loop()
{
	KSF_RUN_APP_BLOCKING_LOOPED(raesp::RaespDevice)
	KSF_RUN_APP_BLOCKING_LOOPED(config::RaespDeviceConfig)
}