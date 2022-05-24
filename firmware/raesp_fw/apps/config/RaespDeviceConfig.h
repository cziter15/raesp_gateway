#pragma once
#include <ksIotFrameworkLib.h>

class RaespDeviceConfig : public ksf::ksApplication
{
	public:
		static const char RaespDeviceName[];

		bool init() override;
		bool loop() override;
};