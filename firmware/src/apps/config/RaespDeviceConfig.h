#pragma once
#include <ksIotFrameworkLib.h>

namespace config
{
	class RaespDeviceConfig : public ksf::ksApplication
	{
		public:
			static const char RaespDeviceName[];

			bool init() override;
			bool loop() override;
	};
}