# DroneLTM-Reader

**Experimental, Use at Your Own Risk**

This repository contains the firmware and configuration for **DroneLTM-Reader**, a lightweight telemetry (LTM) data reader running on an **ESP8266**. It is designed to interface with a flight controller and process real-time telemetry data via the LTM protocol.

## Prerequisites

- **ESP8266** development board.
- Flight controller with LTM telemetry output enabled.
- **Arduino Studio** installed and configured for ESP8266 development.
- Basic understanding of the LTM protocol and drone telemetry data.

## Overview

**DroneLTM-Reader** enables real-time reading and processing of telemetry data from a flight controller over the LTM protocol. The firmware parses the telemetry packets and provides access to essential flight data like position, altitude, speed, and battery levels.

### Key Features:
- Supports **LTM Frame Types**: GPS, Attitude, Status, and other essential frames.
- Efficient data parsing for minimal processing overhead.
- Configurable parameters for baud rate and telemetry output.
- Lightweight design optimized for ESP8266.
- Outputs processed data via Serial or WiFi for additional integration.

## Application Configuration

### LTM Protocol Support:
- **GPS Frame**: Provides latitude, longitude, altitude, speed, and satellites.
- **Attitude Frame**: Captures pitch, roll, and heading.
- **Status Frame**: Includes voltage, current, and flight mode.

### Firmware Settings:
- **Baud Rate**: Default set to 9600 (can be configured).
- **Telemetry Output**: Options for Serial or WiFi.
