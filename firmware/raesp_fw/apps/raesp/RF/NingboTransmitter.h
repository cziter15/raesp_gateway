#ifndef NingboTransmitter_h
#define NingboTransmitter_h

class NingboTransmitter
{
  public:
	NingboTransmitter(int8_t tx_pin, int8_t led_pin);
	void setSwitch(uint32_t transmitterId, bool enabled) const;

  private:
	int8_t txPin;
	int8_t ledPin;
	int8_t repeat;

	void sendBitArray(const char* bitArrayStr) const;
	void sendBit(const char bitChar) const;

};

#endif
