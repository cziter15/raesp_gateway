/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the RaespDevice (WiFi 433 MHz bridge) firmware.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#include "Arduino.h"
#include "nexa.h"

#define PULSE_TIME 296

namespace apps::raesp::protocols
{
	void tx_nexa_bit(const proto_pins& pins, bool b)
	{
		proto_high_for(pins, PULSE_TIME);
		proto_low_for(pins, b ? PULSE_TIME * 5 : PULSE_TIME);
	}

	void tx_nexa_pair(const proto_pins& pins, bool b)
	{
		// Send the Manchester Encoded data 01 or 10, never 11 or 00
		tx_nexa_bit(pins, b);
		tx_nexa_bit(pins, !b);
	}

	void tx_nexa_switch(const proto_pins& pins, bool blnOn, uint32_t transmitterId, int8_t recipient, int8_t level)
	{
		// start pulse
		proto_high_for(pins, PULSE_TIME);
		proto_low_for(pins, PULSE_TIME * 10 + (PULSE_TIME >> 1));

		// Send Device Address..
		for (int8_t i{25}; i >= 0; --i)
			tx_nexa_pair(pins, (uint32_t)(transmitterId & ((uint32_t)1 << i)) != 0);

		// Send 26th bit - group 1/0
		tx_nexa_pair(pins, recipient == -1);

		if (level > 0)
		{
			// dimmer level was given send 00
			tx_nexa_bit(pins, false);
			tx_nexa_bit(pins, false);
		}
		else
		{
			// Send 27th bit - on/off 1/0
			tx_nexa_pair(pins, blnOn);
		}

		// 4 bits - recipient
		for (int8_t i{3}; i >= 0; --i)
			tx_nexa_pair(pins, (recipient & (1 << i)) != 0);

		if (level > 0)
			for (int8_t i{3}; i >= 0; --i)
				tx_nexa_pair(pins, (level & (1 << i)) != 0);

		// stop pulse
		proto_high_for(pins, PULSE_TIME);
		proto_low_for(pins, PULSE_TIME * 40 - 1000); //decrease by one ms (app tick)
	}
}