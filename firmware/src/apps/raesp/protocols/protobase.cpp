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

		GPIO_REG_WRITE(
			GPIO_DRIVE_STRENGTH_REG, 
			(GPIO_REG_READ(GPIO_DRIVE_STRENGTH_REG) & 0xfffff300) | 
			(GPIO_DRIVE_STRENGTH_LOW & 0xff)
		);
	}

	void proto_low_for(const proto_pins& pins, uint32_t us)
	{
		GPOC = (1 << pins.tx);
		GPOC = (1 << pins.led);
		os_delay_us(us);
	}

	void proto_high_for(const proto_pins& pins, uint32_t us)
	{
		GPOS = (1 << pins.tx);
		GPOS = (1 << pins.led);
		os_delay_us(us);
	}
}