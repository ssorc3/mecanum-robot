#ifndef HEARTBEAT_LED_H
#define HEARTBEAT_LED_H

#include <Arduino.h>

#include "util/periodic_timer.h"

class HeartbeatLed {
public:
    explicit HeartbeatLed(uint32_t period_ms)
        : timer_(period_ms), state_(false) {}

    void begin() {
        pinMode(LED_BUILTIN, OUTPUT);
        state_ = false;
        digitalWrite(LED_BUILTIN, state_ ? HIGH : LOW);
        timer_.reset(0);
    }

    void update(uint32_t now_ms) {
        if (!timer_.due(now_ms)) {
            return;
        }
        state_ = !state_;
        digitalWrite(LED_BUILTIN, state_ ? HIGH : LOW);
    }

private:
    PeriodicTimer timer_;
    bool state_;
};

#endif // HEARTBEAT_LED_H
