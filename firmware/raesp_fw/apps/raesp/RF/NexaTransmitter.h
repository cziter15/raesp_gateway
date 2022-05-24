#ifndef NexaTransmitter_h
#define NexaTransmitter_h

class NexaTransmitter
{
  public:
    NexaTransmitter(int8_t tx_pin, int8_t led_pin);
	void setSwitch(bool on, uint32_t transmitterId, int8_t recipient, int8_t level = 0);

  private:
	int8_t txPin;
	int8_t ledPin;
	int8_t repeat;

	void transmit(bool, uint32_t, int8_t, int8_t);
	void sendBit(bool);
	void sendPair(bool);
};

#endif
