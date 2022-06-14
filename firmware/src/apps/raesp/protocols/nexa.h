#pragma once
#include <inttypes.h>
#include "protobase.h"

namespace raesp
{
	namespace protocols
	{
		extern void tx_nexa_switch(const proto_pins& pins, bool blnOn, uint32_t transmitterId, int8_t recipient, int8_t level = 0);
	}
}