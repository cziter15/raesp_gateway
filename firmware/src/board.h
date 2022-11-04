/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#pragma once

#define CFG_WIFI_LED 0
#define CFG_RADIO_LED 1
#define CFG_NSS_PIN 16
#define CFG_DIO0_PIN 5
#define CFG_RST_PIN 15
#define CFG_DIO2_PIN 4

#define TRANSMIT_POWER_DBM 13
#define TRANSMIT_FREQ 433.928

#ifdef USING_TEMP_SENSOR
	/* Share temp sensor pins with other functions. */
	#define TEMP_DATA_PIN CFG_WIFI_LED
	/* Share temp sensor pins with other functions. */
	#define TEMP_ENAB_PIN CFG_RADIO_LED
	/* Resolution for measurement. Higher resulution means bigger delay. */
	#define TEMP_SENSOR_RESOLUTION 11
	/* Update interval. Currently five minutes (300000 ms). */
	#define TEMP_UPDATE_INTERVAL_MS 300000UL
#endif