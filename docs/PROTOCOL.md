# Control + Telemetry Protocol (UDP)

This document describes the binary UDP control and telemetry protocol for the mecanum robot. It is intentionally small and extensible.

## Transport

- UDP on the configured listen port.
- The robot learns the sender IP/port from the first valid packet and replies (telemetry) to that address.

## Byte Order

- All multi-byte integers are little-endian.

## Frame Format

```
+--------+--------+---------+----------+------+------------+
| magic0 | magic1 | version | msg_type | seq  | payload_len|
+--------+--------+---------+----------+------+------------+
|   1    |   1    |    1    |    1     |  1   |      1     |
```

- magic: ASCII "MR" (0x4D, 0x52)
- version: protocol version, currently 1
- msg_type: 0x01 for command, 0x02 for telemetry
- seq: incrementing sequence number (0-255). Receiver may ignore.
- payload_len: length in bytes of the payload that follows

Payload bytes follow immediately after the header.

## Payload Format (TLV)

The payload is a list of TLVs:

```
+------+-----+-------...
| type | len | value
+------+-----+-------...
|  1   |  1  |  len bytes
```

- type: command type
- len: number of bytes in value
- value: command data

Unknown TLV types must be ignored by the receiver (skip len bytes) to preserve forward compatibility.

## Command TLVs

### 0x01 - Drive Vector

```
value (6 bytes):
+------+------+------+
|  vx  |  vy  |  wz  |
+------+------+------+
| int16| int16| int16|
```

- vx: forward/backward velocity
- vy: left/right strafe velocity
- wz: yaw rate (positive = rotate left)
- Scaling (normalized fixed-point int16):
  - vx, vy, wz are in the range -255..255
  - values represent -1.000 to 1.000 (divide by 255)
  - wz is positive for left rotation

## Telemetry TLVs

Telemetry uses the same TLV format and the telemetry message type (0x02). TLVs are optional; send only what is available.

### 0x10 - Battery Voltage

```
value (2 bytes): uint16
```

- battery_mv: battery voltage in millivolts

### 0x11 - Wi-Fi RSSI

```
value (1 byte): int8
```

- rssi_dbm: signal strength in dBm

### 0x12 - Uptime

```
value (4 bytes): uint32
```

- uptime_ms: milliseconds since boot

### 0x13 - Error Flags

```
value (2 bytes): uint16
```

- error_flags: bitfield reserved for faults

## Validity Rules

A frame is valid if:
- magic bytes match
- version is 1
- msg_type is 0x01 or 0x02
- payload_len does not exceed the received packet length
- TLVs are well-formed (no overrun)

## Safety

Recommended behavior on the robot:
- If no valid command is received within a short timeout (for example 300 ms), stop motion (vx=vy=wz=0).

## Example (hex)

Drive forward at full speed, no strafe, no yaw:

```
4D 52 01 01 2A 08  01 06  FF 00  00 00  00 00
```

Header:
- 4D 52: "MR"
- 01: version
- 01: command
- 2A: sequence
- 08: payload length

TLV:
- 01: drive vector
- 06: length
- FF 00: vx = 255 (normalized 1.000)
- 00 00: vy = 0
- 00 00: wz = 0
