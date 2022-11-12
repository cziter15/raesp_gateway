/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#pragma once

#include "protobase.h"

namespace apps::raesp::protocols
{
	extern void tx_nexa_switch(const proto_pins& pins, bool blnOn, uint32_t transmitterId, int8_t recipient, int8_t level = 0);
}