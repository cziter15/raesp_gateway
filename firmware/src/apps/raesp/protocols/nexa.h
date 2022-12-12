/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the RaespDevice (WiFi 433 MHz bridge) firmware.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */
 
#pragma once

#include "protobase.h"

namespace apps::raesp::protocols
{
		/*
		Blocking function that will emit nexa protocol signal on passed tx pin.
		Will also drive passed led pin as transmission indicator.

		@param pins Pin structure reference (containing pin numbers).
		@param blOn True if switch should be enabled, false if disabled.
		@param transmitterId ID of the transmitter (remote).
		@param groupId Group ID/number.
		@param level Optional dimmer level (in case that groupId is a dimmer switch).
	*/
	extern void tx_nexa_switch(const proto_pins& pins, bool blnOn, uint32_t transmitterId, int8_t groupId, int8_t level = 0);
}