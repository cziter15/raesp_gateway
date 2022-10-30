/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#include "Arduino.h"
#include "protobase.h"

namespace apps::raesp::protocols
{
	void proto_low_for(const proto_pins& pins, uint32_t us)
	{
		digitalWrite(pins.tx, LOW);	
		digitalWrite(pins.led, LOW);
		os_delay_us(us);
	}

	void proto_high_for(const proto_pins& pins, uint32_t us)
	{
		digitalWrite(pins.tx, HIGH);	
		digitalWrite(pins.led, HIGH);
		os_delay_us(us);
	}
}