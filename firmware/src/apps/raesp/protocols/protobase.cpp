#include "Arduino.h"
#include "protobase.h"
namespace raesp
{
	namespace protocols
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
}