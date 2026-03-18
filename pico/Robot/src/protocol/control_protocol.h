#ifndef CONTROL_PROTOCOL_H
#define CONTROL_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

const uint8_t FRAME_MAGIC0 = 'M';
const uint8_t FRAME_MAGIC1 = 'R';
const uint8_t PROTOCOL_VERSION = 1;

const uint8_t MSG_TYPE_COMMAND = 0x01;
const uint8_t MSG_TYPE_TELEMETRY = 0x02;

const uint8_t TLV_TYPE_DRIVE = 0x01;

const uint8_t TLV_TYPE_BATTERY_MV = 0x10;
const uint8_t TLV_TYPE_RSSI_DBM = 0x11;
const uint8_t TLV_TYPE_UPTIME_MS = 0x12;
const uint8_t TLV_TYPE_ERROR_FLAGS = 0x13;

const size_t CONTROL_HEADER_SIZE = 6;

struct ControlCommand {
    bool has_drive;
    int16_t vx;
    int16_t vy;
    int16_t wz;
};

struct TelemetryData {
    bool has_battery_mv;
    uint16_t battery_mv;

    bool has_rssi_dbm;
    int8_t rssi_dbm;

    bool has_uptime_ms;
    uint32_t uptime_ms;

    bool has_error_flags;
    uint16_t error_flags;
};

void resetControlCommand(ControlCommand& cmd);
void resetTelemetryData(TelemetryData& tlm);

bool parseControlFrame(const uint8_t* data,
                       size_t length,
                       ControlCommand& out_cmd,
                       uint8_t& out_seq);

size_t encodeTelemetryFrame(uint8_t* out,
                            size_t max_len,
                            uint8_t seq,
                            const TelemetryData& tlm);

#endif // CONTROL_PROTOCOL_H
