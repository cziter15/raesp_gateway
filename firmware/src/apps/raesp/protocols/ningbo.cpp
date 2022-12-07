/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the RaespDevice (WiFi 433 MHz bridge) firmware.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#include "Arduino.h"
#include "ningbo.h"

#define PULSE_SHORT_A 280
#define PULSE_SHORT_B 360
#define PULSE_LONG_A 925
#define PULSE_LONG_B 1000
#define PULSE_ULTRA_LONG 10000

namespace apps::raesp::protocols
{
	void tx_ningbo_bit(const proto_pins& pins, const char bitChar)
	{
		bool isBitCharOne = bitChar == '1';
		if (bitChar == '0' || isBitCharOne)
		{
			proto_high_for(pins,PULSE_SHORT_A);
			proto_low_for(pins, PULSE_LONG_B);
			
			proto_high_for(pins, isBitCharOne ? PULSE_SHORT_A : PULSE_LONG_A);
			proto_low_for(pins, isBitCharOne ? PULSE_LONG_B : PULSE_SHORT_B);
		}
		else
		{
			bool isBitCharH = bitChar == 'H';
			if (bitChar == 'L' || isBitCharH)
			{
				for (int8_t i{0}; i < 2; i++)
				{
					proto_high_for(pins, isBitCharH ? PULSE_LONG_A : PULSE_SHORT_A);
					proto_low_for(pins, isBitCharH ? PULSE_SHORT_B : PULSE_LONG_B);
				}

				proto_high_for(pins, PULSE_SHORT_A);
				proto_low_for(pins, PULSE_ULTRA_LONG);
			}
		}
	}

	void tx_ningbo_bitarrray(const proto_pins& pins, const char * bitArrayStr)
	{
		for (; *bitArrayStr != 0; ++bitArrayStr)
			tx_ningbo_bit(pins, *bitArrayStr);
	}

	void tx_ningbo_switch(const proto_pins& pins, bool blnOn, uint32_t switchId)
	{
		proto_high_for(pins, PULSE_SHORT_A);
		proto_low_for(pins, PULSE_ULTRA_LONG);

		tx_ningbo_bitarrray(pins, "1000");

		for (int8_t b{0}; b < 7; ++b)
			tx_ningbo_bit(pins, ((uint32_t)(switchId & ((uint32_t)1 << b)) != 0) ? '1' : '0');

		tx_ningbo_bit(pins, blnOn ? 'H' : 'L');
	}
}