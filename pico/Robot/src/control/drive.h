#ifndef DRIVE_H
#define DRIVE_H

#include <stdint.h>

#include "protocol/control_protocol.h"
#include "util/periodic_timer.h"

class DriveController {
public:
    explicit DriveController(uint32_t period_ms);

    void begin();
    void acceptCommand(const ControlCommand& cmd, uint32_t now_ms);
    void update(uint32_t now_ms);

private:
    PeriodicTimer timer_;
    ControlCommand last_command_;
    uint32_t last_command_ms_;
};

#endif // DRIVE_H
