/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the RaespDevice (WiFi 433 MHz bridge) firmware.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#pragma once

#include <inttypes.h>

namespace apps::raesp::protocols
{
	struct proto_pins
	{
		uint8_t tx;
		uint8_t led;
	};

	extern void proto_high_for(const proto_pins& pins, uint32_t us);
	extern void proto_low_for(const proto_pins& pins, uint32_t us);
}