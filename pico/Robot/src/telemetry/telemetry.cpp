#include "telemetry.h"

#include "protocol/control_protocol.h"
#include "util/log.h"

void TelemetryService::update(UDPHandler& udp, uint32_t now_ms) {
    if (!timer_.due(now_ms)) {
        return;
    }

    TelemetryData tlm;
    resetTelemetryData(tlm);
    tlm.has_rssi_dbm = true;
    tlm.rssi_dbm = static_cast<int8_t>(udp.getSignalStrength());
    tlm.has_uptime_ms = true;
    tlm.uptime_ms = now_ms;

    uint8_t telemetry_buffer[32];
    size_t telemetry_size = encodeTelemetryFrame(telemetry_buffer, sizeof(telemetry_buffer), seq_, tlm);
    if (telemetry_size == 0) {
        logPrintln("[TLM] Failed to encode telemetry frame");
        return;
    }

    if (udp.sendTelemetry(telemetry_buffer, telemetry_size)) {
        logPrint("[TLM] Sent seq=");
        logPrint(seq_);
        logPrint(" size=");
        logPrint(telemetry_size);
        logPrint(" rssi=");
        logPrint(tlm.rssi_dbm);
        logPrint(" uptime=");
        logPrintln(tlm.uptime_ms);
        seq_++;
    } else {
        logPrintln("[TLM] Telemetry send skipped or failed");
    }
}
