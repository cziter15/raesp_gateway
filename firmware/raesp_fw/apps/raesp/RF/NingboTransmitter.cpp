/*
 * Krzysztof Strehlau 13/06/2021
 * Handles Ningbo power outlets / EverFlourish
 */

#include "Arduino.h"
#include "NingboTransmitter.h"
#include "RFMacros.h"

#define PULSE_SHORT_A 280
#define PULSE_SHORT_B 360
#define PULSE_LONG_A 925
#define PULSE_LONG_B 1000
#define PULSE_ULTRA_LONG 10000

NingboTransmitter::NingboTransmitter(int8_t tx_pin, int8_t led_pin) :
	txPin(tx_pin), ledPin(led_pin), repeat(9)
{
	pinMode(txPin, OUTPUT);		// transmitter pin.
	pinMode(ledPin, OUTPUT);	// led indicator pin

	txLow();
}

void IRAM_ATTR NingboTransmitter::sendBit(const char bitChar) const
{
	bool isHigh = bitChar == '1';
	if (bitChar == '0' || isHigh)
	{
		txHighFor(PULSE_SHORT_A);
		txLowFor(PULSE_LONG_B);
		
		txHighFor(isHigh ? PULSE_SHORT_A : PULSE_LONG_A);
		txLowFor(isHigh ? PULSE_LONG_B : PULSE_SHORT_B);
	}
	else
	{
		bool isHigh = bitChar == 'H';
		if (bitChar == 'L' || isHigh)
		{
			for (int8_t i = 0; i < 2; i++)
			{
				txHighFor(isHigh ? PULSE_LONG_A : PULSE_SHORT_A);
				txLowFor(isHigh ? PULSE_SHORT_B : PULSE_LONG_B);
			}

			txHighFor(PULSE_SHORT_A);
			txLowFor(PULSE_ULTRA_LONG);
		}
	}
}

void IRAM_ATTR NingboTransmitter::sendBitArray(const char * bitArrayStr) const
{
	for (; *bitArrayStr != 0; ++bitArrayStr)
		sendBit(*bitArrayStr);
}

void IRAM_ATTR NingboTransmitter::setSwitch(uint32_t transmitterId, bool enabled) const
{
	noInterrupts();

	txHighFor(PULSE_SHORT_A);
	txLowFor(PULSE_ULTRA_LONG);
	
	for (uint8_t i = 0; i < repeat; ++i)
	{
		sendBitArray("1000");

		for (int8_t b = 0; b < 7; ++b)
		  sendBit(((uint32_t)(transmitterId & ((uint32_t)1 << b)) != 0) ? '1' : '0');

		sendBit(enabled ? 'H' : 'L');
	}

	interrupts();
	delay(1);
}