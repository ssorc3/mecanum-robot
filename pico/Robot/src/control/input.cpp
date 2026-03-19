#include "input.h"

#include "config/config.h"

static float clampAbs(float value, float limit) {
    if (value > limit) {
        return limit;
    }
    if (value < -limit) {
        return -limit;
    }
    return value;
}

float scaleAxis(int16_t value) {
    if (CONTROL_INPUT_MAX <= 0) {
        return 0.0f;
    }

    float scaled = static_cast<float>(value) / static_cast<float>(CONTROL_INPUT_MAX);
    return clampAbs(scaled, 1.0f);
}
