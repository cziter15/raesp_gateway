# raesp_gateway
Ra-02 (SX1278) + ESP12 (ESP8266) gateway

# Project goal
Few years ago, when I've started with designing electronics for fun, I've created something like WiFi Controlled 433 remote for RC power outlets that I've bought from the store. First iteration was something very bad, when I've had no idea how to design PCB, just connected all the pins on two layer board and poured layers with GND. It worked, but not as stable as expected. Then I've redesigned the board, with respect to minimize current loops, but still... not everything working as expected. More info  [here](https://hackaday.io/project/163833-wifi-to-433-mhz-bridge).

I've decided to switch to Ra-02 instead of RFM110W. It's theoretically a lot better, because it's programmable and is not only transmitter, but transceiver. That means that I can receive and send messages and I'm not limited to OOK as it even supports LoRA, FSK and more!
