#include "apps/raesp/RaespDevice.h"
#include "apps/config/RaespDeviceConfig.h"

// the setup function runs once when you press reset or power the board
void setup()
{
	KSF_FRAMEWORK_INIT()
}

// the loop function runs over and over again until power down or reset
void loop()
{
	KSF_RUN_APP_BLOCKING_LOOPED(raesp::RaespDevice)
	KSF_RUN_APP_BLOCKING_LOOPED(config::RaespDeviceConfig)
}