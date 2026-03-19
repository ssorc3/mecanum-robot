#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdint.h>

#include "network/udp_handler.h"
#include "util/periodic_timer.h"

class TelemetryService {
public:
    explicit TelemetryService(uint32_t period_ms)
        : timer_(period_ms), seq_(0) {}

    void update(UDPHandler& udp, uint32_t now_ms);

private:
    PeriodicTimer timer_;
    uint8_t seq_;
};

#endif // TELEMETRY_H
