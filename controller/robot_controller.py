#!/usr/bin/env python3
import argparse
import socket
import struct
import sys
import time

try:
    import pygame
except ImportError:
    pygame = None


MAGIC = b"MR"
VERSION = 1
MSG_TYPE_COMMAND = 0x01
MSG_TYPE_TELEMETRY = 0x02
TLV_DRIVE = 0x01


def clamp(value, lo, hi):
    return max(lo, min(hi, value))


def scale_axis(raw, deadzone):
    if abs(raw) < deadzone:
        return 0.0
    return clamp(raw, -1.0, 1.0)


def axis_to_int16(value):
    return int(round(clamp(value, -1.0, 1.0) * 255))


def build_drive_packet(seq, vx, vy, wz):
    payload = struct.pack("<BBhhh", TLV_DRIVE, 6, vx, vy, wz)
    header = struct.pack("<2sBBBB", MAGIC, VERSION, MSG_TYPE_COMMAND, seq, len(payload))
    return header + payload


def parse_telemetry(data):
    if len(data) < 6:
        return None
    magic0, magic1, version, msg_type, seq, payload_len = struct.unpack("<BBBBBB", data[:6])
    if bytes([magic0, magic1]) != MAGIC:
        return None
    if version != VERSION or msg_type != MSG_TYPE_TELEMETRY:
        return None
    if len(data) < 6 + payload_len:
        return None

    payload = data[6:6 + payload_len]
    offset = 0
    items = []
    while offset + 2 <= len(payload):
        tlv_type = payload[offset]
        tlv_len = payload[offset + 1]
        offset += 2
        if offset + tlv_len > len(payload):
            break
        value = payload[offset:offset + tlv_len]
        offset += tlv_len

        if tlv_type == 0x10 and tlv_len == 2:
            (battery_mv,) = struct.unpack("<H", value)
            items.append(f"battery_mv={battery_mv}")
        elif tlv_type == 0x11 and tlv_len == 1:
            (rssi_dbm,) = struct.unpack("<b", value)
            items.append(f"rssi_dbm={rssi_dbm}")
        elif tlv_type == 0x12 and tlv_len == 4:
            (uptime_ms,) = struct.unpack("<I", value)
            items.append(f"uptime_ms={uptime_ms}")
        elif tlv_type == 0x13 and tlv_len == 2:
            (error_flags,) = struct.unpack("<H", value)
            items.append(f"error_flags=0x{error_flags:04X}")

    if not items:
        return None
    return ", ".join(items)


def wait_for_joystick():
    while True:
        pygame.joystick.quit()
        pygame.joystick.init()
        if pygame.joystick.get_count() > 0:
            js = pygame.joystick.Joystick(0)
            js.init()
            return js
        print("No controller detected. Waiting...", flush=True)
        time.sleep(1.0)


def main():
    parser = argparse.ArgumentParser(description="Xbox controller UDP sender for mecanum robot")
    parser.add_argument("--ip", required=True, help="Robot IP address")
    parser.add_argument("--port", type=int, required=True, help="Robot UDP port")
    parser.add_argument("--rate", type=float, default=20.0, help="Send rate in Hz (default: 20)")
    parser.add_argument("--deadzone", type=float, default=0.15, help="Stick deadzone (0-1)")
    parser.add_argument("--left-x-axis", type=int, default=0, help="Left stick X axis index")
    parser.add_argument("--left-y-axis", type=int, default=1, help="Left stick Y axis index")
    parser.add_argument("--right-x-axis", type=int, default=3, help="Right stick X axis index")
    args = parser.parse_args()

    if pygame is None:
        print("pygame is required. Install with: pip install -r requirements.txt", file=sys.stderr)
        return 1

    pygame.init()
    pygame.joystick.init()
    joystick = wait_for_joystick()

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(("", args.port))
    sock.setblocking(False)

    telemetry_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    telemetry_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    telemetry_sock.bind(("", 4211))
    telemetry_sock.setblocking(False)

    target = (args.ip, args.port)
    period = 1.0 / args.rate
    seq = 0

    print(f"Sending to {args.ip}:{args.port} at {args.rate} Hz", flush=True)

    next_send = time.monotonic()
    while True:
        pygame.event.pump()

        left_x = joystick.get_axis(args.left_x_axis)
        left_y = joystick.get_axis(args.left_y_axis)
        right_x = joystick.get_axis(args.right_x_axis)

        vx = axis_to_int16(scale_axis(-left_y, args.deadzone))
        vy = axis_to_int16(scale_axis(left_x, args.deadzone))
        wz = axis_to_int16(scale_axis(right_x, args.deadzone))

        now = time.monotonic()
        if now >= next_send:
            packet = build_drive_packet(seq, vx, vy, wz)
            sock.sendto(packet, target)
            seq = (seq + 1) & 0xFF
            next_send += period

        while True:
            try:
                data, _addr = telemetry_sock.recvfrom(2048)
            except BlockingIOError:
                break
            telemetry = parse_telemetry(data)
            if telemetry:
                print(f"telemetry: {telemetry}", flush=True)

        sleep_time = max(0.0, next_send - time.monotonic())
        time.sleep(min(0.005, sleep_time))


if __name__ == "__main__":
    raise SystemExit(main())
