# Firmware : raesp_gateway

This directory contains example firmware for RaEsp gateway device. At this moment only example one-way bridge is implemented `WiFi -> OOK`, but hardware can be used to build more advanced things.

## Features
- Supports transmit protocol for Nexa remote power outlets.
- Supports transmit protocol for Ningbo remote power outlets.
- Supports DS18B20 for room temperature measurement.
- MQTT connectivity

## Compoonents
- RadioCommander - provides Radio Command interface, this component handles message commands coming from MQTT and then handles them by sending apropriate RF messages to remote controlled power outlets.
- TempSensor - optional temperature sensor support (DS18B20) that can be connected to flash/debug header on PCB. Uses RX pin [CFG_RADIO_LED/GPIO1] as power supply and BT [CFG_WIFI_LED/GPIO0] as communication pin.

## Protocols
- Nexa - Nexa RF OOK protocol implementation.
- Ningbo - Ningbo RF OOK protocol implementation.
