# Mecanum Robot

Mecanum Robot is a Raspberry Pi Pico W car platform controlled over UDP. It uses mecanum wheels to support holonomic movement (forward/backward, strafe, rotate) and is built with PlatformIO.

## Features

- UDP control path with a dedicated handler
- Mecanum wheel drive for omnidirectional motion

## Status

- UDP transport is implemented
- Control protocol is documented

## Configuration

Set your Wi-Fi credentials in [src/config/wifi_credentials.h](src/config/wifi_credentials.h). This file is gitignored so passwords are not committed.

## Control + Telemetry Protocol

See [PROTOCOL.md](PROTOCOL.md) for the binary UDP control and telemetry protocol specification.

## Project Layout

- [src/main.cpp](src/main.cpp): firmware entry point
- [src/config/](src/config/): configuration headers (including Wi-Fi credentials)
- [network/](network/): UDP handler implementation


## Motor layout
| Pico Pin | Description |
|----------|-------------|
| GPIO18   | M1_IN1      |
| GPIO19   | M1_IN2      |
| GPIO20   | M2_IN1      |
| GPIO21   | M2_IN2      |
| GPIO6    | M3_IN1      |
| GPIO7    | M3_IN2      |
| GPIO8    | M4_IN1      |
| GPIO9    | M4_IN2      |

M1 is front left
M2 is back left
M3 is front right
M4 is back right

| Mx_IN1 | Mx_IN2 | Direction |
|--------|--------|-----------|
| 1      | 0      | forward   |
| 0      | 1      | backward  |