# Firmware : raesp_gateway

This directory contains example firmware for RaEsp gateway device. At this moment only example one-way bridge is implemented `WiFi -> OOK`, but hardware can be used to build more advanced things.

## Features
- Supports transmit protocol for Nexa remote power outlets.
- Supports transmit protocol for Ningbo remote power outlets.
- MQTT connectivity

## Compoonents
- RadioCommander - provides Radio Command interface, this component handles message commands coming from MQTT and then handles them by sending apropriate RF messages to remote controlled power outlets.

## Protocols
- Nexa - Nexa RF OOK protocol implementation.
- Ningbo - Ningbo RF OOK protocol implementation.