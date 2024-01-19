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
	static inline volatile uint32_t asm_ccount(void) 
	{
		uint32_t r;
		asm volatile ("rsr %0, ccount" : "=r"(r));
		return r;
	}

	void IRAM_ATTR delayTicks(int32_t ticks) 
	{
		uint32_t expire_ticks {asm_ccount() + ticks -13};

		do 
			ticks = expire_ticks - asm_ccount();
		while (ticks >= 6 );

		asm volatile(
			"blti %0, 1 , 0f;"
			"beqi %0, 1 , 0f;"
			"beqi %0, 2 , 0f;"
			"beqi %0, 3 , 0f;"
			"beqi %0, 4 , 0f;"
			"bgei %0, 5 , 0f;"
			"0:;" : : "r"(ticks)
		);
	}

	void IRAM_ATTR fixed_delay_us(int32_t us) 
	{
		delayTicks(80 * us - 13);
	}

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
		fixed_delay_us(us);
	}

	void proto_high_for(const proto_pins& pins, uint32_t us)
	{
		GPOS = (1 << pins.tx);
		if (pins.led != -1)
			GPOS = (1 << pins.led);
		fixed_delay_us(us);
	}
}