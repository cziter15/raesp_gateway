#include "apps/raesp/RaespDevice.h"
#include "apps/config/RaespDeviceConfig.h"

// the setup function runs once when you press reset or power the board
void setup()
{
	ksf::initKsfFramework();
}

// the loop function runs over and over again until power down or reset
void loop()
{
	RUN_APP_BLOCKING_LOOPED(RaespDevice)
	RUN_APP_BLOCKING_LOOPED(RaespDeviceConfig)
}