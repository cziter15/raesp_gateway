/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the RaespDevice (WiFi 433 MHz bridge) firmware.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#include "Arduino.h"
#include "protobase.h"
#include "../../../board.h"

namespace apps::raesp::protocols
{
	void proto_low_for(const proto_pins& pins, uint32_t us)
	{
		#if TX_WITH_PULLUP
			GPES = (1 << pins.tx);
		#else
			GPOC = (1 << pins.tx);
		#endif

		GPOC = (1 << pins.led);
		os_delay_us(us);
	}

	void proto_high_for(const proto_pins& pins, uint32_t us)
	{
		#if TX_WITH_PULLUP
			GPEC = (1 << pins.tx);
		#else
			GPOS = (1 << pins.tx);
		#endif

		GPOS = (1 << pins.led);
		os_delay_us(us);
	}
}