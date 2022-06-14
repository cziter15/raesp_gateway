#pragma once
#include <inttypes.h>
#include "protobase.h"

namespace raesp
{
	namespace protocols
	{
		extern void tx_ningbo_switch(const proto_pins& pins, bool blnOn, uint32_t switchId);
	}
}