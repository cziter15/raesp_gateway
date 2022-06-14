#pragma once
#include <inttypes.h>
#include "protobase.h"

namespace raesp
{
	namespace protocols
	{
		extern void ningbo_transmit(const proto_pins& pins, bool blnOn, uint32_t transmitterId);
	}
}