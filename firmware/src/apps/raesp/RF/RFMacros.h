#ifndef RF_MACROS
#define RF_MACROS

#define wait_us(x) os_delay_us(x)

#define txHigh()	digitalWrite(txPin, HIGH);	digitalWrite(ledPin, HIGH);
#define txLow()		digitalWrite(txPin, LOW);	digitalWrite(ledPin, LOW);

#define txHighFor(x) txHigh(); wait_us(x);
#define txLowFor(x) txLow(); wait_us(x);

#endif