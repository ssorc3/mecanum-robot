#include "telemetry.h"

#include "protocol/control_protocol.h"

bool sendTelemetryIfDue(UDPHandler& udp,
                        uint32_t now_ms,
                        uint32_t period_ms,
                        uint32_t& last_send_ms,
                        uint8_t& seq) {
    if (now_ms - last_send_ms < period_ms) {
        return false;
    }

    last_send_ms = now_ms;

    TelemetryData tlm;
    resetTelemetryData(tlm);
    tlm.has_rssi_dbm = true;
    tlm.rssi_dbm = static_cast<int8_t>(udp.getSignalStrength());
    tlm.has_uptime_ms = true;
    tlm.uptime_ms = now_ms;

    uint8_t telemetry_buffer[32];
    size_t telemetry_size = encodeTelemetryFrame(telemetry_buffer, sizeof(telemetry_buffer), seq, tlm);
    if (telemetry_size > 0) {
        udp.sendTelemetry(telemetry_buffer, telemetry_size);
        seq++;
        return true;
    }

    return false;
}
