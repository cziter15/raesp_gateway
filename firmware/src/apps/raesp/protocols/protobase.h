#pragma once
#include <inttypes.h>

namespace raesp
{
	namespace protocols
	{
		struct proto_pins
		{
			uint8_t tx;
			uint8_t led;
		};

		extern void proto_high_for(const proto_pins& pins, uint32_t us);
		extern void proto_low_for(const proto_pins& pins, uint32_t us);
	}
}