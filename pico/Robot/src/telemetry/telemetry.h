#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdint.h>

#include "network/udp_handler.h"

bool sendTelemetryIfDue(UDPHandler& udp,
                        uint32_t now_ms,
                        uint32_t period_ms,
                        uint32_t& last_send_ms,
                        uint8_t& seq);

#endif // TELEMETRY_H
