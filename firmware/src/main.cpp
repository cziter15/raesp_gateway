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

KSF_IMPLEMENT_APP_ROTATOR_INTERVAL
(
	10,
	raesp::RaespDevice,
	config::RaespDeviceConfig
)