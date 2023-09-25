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
	void proto_prepare_txpin(uint8_t txpin)
	{
		pinMode(txpin, OUTPUT);
		GPOC = (1 << txpin);
	}

	void proto_low_for(const proto_pins& pins, uint32_t us)
	{
		GPOC = (1 << pins.tx);
		if (pins.led != -1)
			GPOC = (1 << pins.led);
		os_delay_us(us);
	}

	void proto_high_for(const proto_pins& pins, uint32_t us)
	{
		GPOS = (1 << pins.tx);
		if (pins.led != -1)
			GPOS = (1 << pins.led);
		os_delay_us(us);
	}
}