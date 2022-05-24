/*
 * David Edmundson 21/11/2009
 * Krzysztof Strehlau 11/01/2020
 * Based heavily on demo code by
 * Martyn Henderson 02/10/2009  http://martgadget.blogspot.com
 */

#include "Arduino.h"
#include "NexaTransmitter.h"
#include "RFMacros.h"

#define PULSE_TIME	296

NexaTransmitter::NexaTransmitter(int8_t tx_pin, int8_t led_pin) :
	txPin(tx_pin), ledPin(led_pin), repeat(6)
{
	pinMode(txPin, OUTPUT);		// transmitter pin.
	pinMode(ledPin, OUTPUT);	// led indicator pin

	txLow();
}

void IRAM_ATTR NexaTransmitter::setSwitch(bool on, uint32_t transmitterId, int8_t recipient, int8_t level)
{
	noInterrupts();

	// Kick off radio
	txHighFor(100);
	txLowFor(1000);

	// Start TX
	for (int8_t i = 0; i < repeat; ++i)
		transmit(on, transmitterId, recipient, level);

	interrupts();
	delay(1);
}

void IRAM_ATTR NexaTransmitter::transmit(bool blnOn, uint32_t transmitterId, int8_t recipient, int8_t level)
{
	// start pulse
	txHighFor(PULSE_TIME);
	txLowFor(PULSE_TIME * 10 + (PULSE_TIME >> 1));

	// Send Device Address..
	for (int8_t i = 25; i >= 0; --i)
		sendPair((uint32_t)(transmitterId & ((uint32_t)1 << i)) != 0);

	// Send 26th bit - group 1/0
	sendPair(recipient == -1);

	if (level > 0)
	{
		// dimmer level was given send 00
		sendBit(false);
		sendBit(false);
	}
	else
	{
		// Send 27th bit - on/off 1/0
		sendPair(blnOn);
	}

	// 4 bits - recipient
	for (int8_t i = 3; i >= 0; --i)
		sendPair((recipient & (1 << i)) != 0);

	if (level > 0)
		for (int8_t i = 3; i >= 0; --i)
			sendPair((level & (1 << i)) != 0);

	// stop pulse
	txHighFor(PULSE_TIME);
	txLowFor(PULSE_TIME * 40);
}

void IRAM_ATTR NexaTransmitter::sendBit(bool b)
{
	txHighFor(PULSE_TIME);
	txLowFor(b ? PULSE_TIME * 5 : PULSE_TIME);
}

void IRAM_ATTR NexaTransmitter::sendPair(bool b)
{
	// Send the Manchester Encoded data 01 or 10, never 11 or 00
	sendBit(b);
	sendBit(!b);
}