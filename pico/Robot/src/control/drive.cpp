#include "drive.h"

#include "config/config.h"
#include "control/input.h"
#include "control/kinematics.h"
#include "protocol/control_protocol.h"

DriveController::DriveController(uint32_t period_ms)
    : timer_(period_ms), last_command_ms_(0) {
    resetControlCommand(last_command_);
}

void DriveController::begin() {
    initMotors();
    resetControlCommand(last_command_);
    last_command_ms_ = 0;
    timer_.reset(0);
}

void DriveController::acceptCommand(const ControlCommand& cmd, uint32_t now_ms) {
    last_command_ = cmd;
    last_command_ms_ = now_ms;
}

void DriveController::update(uint32_t now_ms) {
    if (!timer_.due(now_ms)) {
        return;
    }

    bool has_recent_command = (now_ms - last_command_ms_) <= COMMAND_TIMEOUT_MS;
    float vx = 0.0f;
    float vy = 0.0f;
    float wz = 0.0f;

    if (has_recent_command && last_command_.has_drive) {
        vx = scaleAxis(last_command_.vx);
        vy = scaleAxis(last_command_.vy);
        wz = scaleAxis(last_command_.wz);
    }

    applyMecanum(vx, vy, wz);
}
