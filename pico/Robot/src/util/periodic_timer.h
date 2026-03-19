#ifndef PERIODIC_TIMER_H
#define PERIODIC_TIMER_H

#include <stdint.h>

class PeriodicTimer {
public:
    explicit PeriodicTimer(uint32_t period_ms)
        : period_ms_(period_ms), last_ms_(0) {}

    bool due(uint32_t now_ms) {
        if (now_ms - last_ms_ >= period_ms_) {
            last_ms_ = now_ms;
            return true;
        }
        return false;
    }

    void reset(uint32_t now_ms = 0) {
        last_ms_ = now_ms;
    }

    uint32_t period() const {
        return period_ms_;
    }

private:
    uint32_t period_ms_;
    uint32_t last_ms_;
};

#endif // PERIODIC_TIMER_H
