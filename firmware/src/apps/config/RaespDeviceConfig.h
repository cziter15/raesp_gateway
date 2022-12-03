/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the RaespDevice (WiFi 433 MHz bridge) firmware.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#pragma once

#include <ksIotFrameworkLib.h>

namespace apps::config
{
	class RaespDeviceConfig : public ksf::ksApplication
	{
		public:
			static const char RaespDeviceName[];	// Device name.

			/* 
				Initializes RaespDeviceConfig application.

				@return True on success, false on fail.
			*/
			bool init() override;

			/* 
				Handles RaespDeviceConfig application.

				@return True on success, false on fail.
			*/
			bool loop() override;
	};
}