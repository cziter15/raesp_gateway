/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the RaespDevice (WiFi 433 MHz bridge) firmware.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/raesp_gateway/blob/main/firmware/LICENSE
 */

#pragma once

/* Pin number used to drive WiFi state indicator (LED). */
#define CFG_WIFI_LED 0
/* Pin number used to drive radio activity indicator (LED). */
#define CFG_RADIO_LED 1
/* Pin number used to drive NSS pad of theradio module interface. */
#define CFG_NSS_PIN 16
/* Pin number used to drive DIO0 pad of the radio module interface. */
#define CFG_DIO0_PIN 5
/* Pin number used to drive RST pad of the radio module interface. */
#define CFG_RST_PIN 15
/* Pin number used to drive DIO2 pad of the radio module interface. */
#define CFG_DIO2_PIN 4

/* Transmit power to set on Ra-02 module. */
#define TRANSMIT_POWER_DBM 13
/* Transmit frequency to set on Ra-02 module*/
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