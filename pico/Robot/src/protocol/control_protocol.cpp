#include "control_protocol.h"

static int16_t readLE16(const uint8_t* data) {
    return static_cast<int16_t>(static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8));
}

static void writeULE16(uint8_t* out, uint16_t value) {
    out[0] = static_cast<uint8_t>(value & 0xFF);
    out[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
}

static void writeULE32(uint8_t* out, uint32_t value) {
    out[0] = static_cast<uint8_t>(value & 0xFF);
    out[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    out[2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    out[3] = static_cast<uint8_t>((value >> 24) & 0xFF);
}

void resetControlCommand(ControlCommand& cmd) {
    cmd.has_drive = false;
    cmd.vx = 0;
    cmd.vy = 0;
    cmd.wz = 0;
}

void resetTelemetryData(TelemetryData& tlm) {
    tlm.has_battery_mv = false;
    tlm.battery_mv = 0;

    tlm.has_rssi_dbm = false;
    tlm.rssi_dbm = 0;

    tlm.has_uptime_ms = false;
    tlm.uptime_ms = 0;

    tlm.has_error_flags = false;
    tlm.error_flags = 0;
}

static bool parseHeader(const uint8_t* data,
                        size_t length,
                        uint8_t expected_msg_type,
                        uint8_t& out_seq,
                        uint8_t& out_payload_len) {
    if (length < CONTROL_HEADER_SIZE) {
        return false;
    }

    if (data[0] != FRAME_MAGIC0 || data[1] != FRAME_MAGIC1) {
        return false;
    }

    if (data[2] != PROTOCOL_VERSION) {
        return false;
    }

    if (data[3] != expected_msg_type) {
        return false;
    }

    out_seq = data[4];
    out_payload_len = data[5];

    if (static_cast<size_t>(out_payload_len) > length - CONTROL_HEADER_SIZE) {
        return false;
    }

    return true;
}

bool parseControlFrame(const uint8_t* data,
                       size_t length,
                       ControlCommand& out_cmd,
                       uint8_t& out_seq) {
    resetControlCommand(out_cmd);

    uint8_t payload_len = 0;
    if (!parseHeader(data, length, MSG_TYPE_COMMAND, out_seq, payload_len)) {
        return false;
    }

    size_t offset = CONTROL_HEADER_SIZE;
    size_t payload_end = CONTROL_HEADER_SIZE + payload_len;

    while (offset + 2 <= payload_end) {
        uint8_t tlv_type = data[offset];
        uint8_t tlv_len = data[offset + 1];
        offset += 2;

        if (offset + tlv_len > payload_end) {
            return false;
        }

        if (tlv_type == TLV_TYPE_DRIVE) {
            if (tlv_len != 6) {
                return false;
            }

            out_cmd.vx = readLE16(data + offset);
            out_cmd.vy = readLE16(data + offset + 2);
            out_cmd.wz = readLE16(data + offset + 4);
            out_cmd.has_drive = true;
        }

        offset += tlv_len;
    }

    return true;
}

static bool writeHeader(uint8_t* out,
                        size_t max_len,
                        uint8_t msg_type,
                        uint8_t seq,
                        uint8_t payload_len) {
    if (max_len < CONTROL_HEADER_SIZE) {
        return false;
    }

    out[0] = FRAME_MAGIC0;
    out[1] = FRAME_MAGIC1;
    out[2] = PROTOCOL_VERSION;
    out[3] = msg_type;
    out[4] = seq;
    out[5] = payload_len;
    return true;
}


size_t encodeTelemetryFrame(uint8_t* out,
                            size_t max_len,
                            uint8_t seq,
                            const TelemetryData& tlm) {
    size_t offset = CONTROL_HEADER_SIZE;

    if (max_len < CONTROL_HEADER_SIZE) {
        return 0;
    }

    if (tlm.has_battery_mv) {
        if (offset + 2 + 2 > max_len) {
            return 0;
        }
        out[offset++] = TLV_TYPE_BATTERY_MV;
        out[offset++] = 2;
        writeULE16(out + offset, tlm.battery_mv);
        offset += 2;
    }

    if (tlm.has_rssi_dbm) {
        if (offset + 2 + 1 > max_len) {
            return 0;
        }
        out[offset++] = TLV_TYPE_RSSI_DBM;
        out[offset++] = 1;
        out[offset++] = static_cast<uint8_t>(tlm.rssi_dbm);
    }

    if (tlm.has_uptime_ms) {
        if (offset + 2 + 4 > max_len) {
            return 0;
        }
        out[offset++] = TLV_TYPE_UPTIME_MS;
        out[offset++] = 4;
        writeULE32(out + offset, tlm.uptime_ms);
        offset += 4;
    }

    if (tlm.has_error_flags) {
        if (offset + 2 + 2 > max_len) {
            return 0;
        }
        out[offset++] = TLV_TYPE_ERROR_FLAGS;
        out[offset++] = 2;
        writeULE16(out + offset, tlm.error_flags);
        offset += 2;
    }

    size_t payload_len = offset - CONTROL_HEADER_SIZE;
    if (payload_len > 255) {
        return 0;
    }

    if (!writeHeader(out, max_len, MSG_TYPE_TELEMETRY, seq, static_cast<uint8_t>(payload_len))) {
        return 0;
    }

    return offset;
}
