#ifndef CONFIG_H
#define CONFIG_H

#include "wifi_config.h"
#include "wifi_credentials.h"

const uint16_t CONTROL_LOOP_FREQUENCY_HZ = 20; // Hz
const uint16_t CONTROL_LOOP_PERIOD_MS = 1000 / CONTROL_LOOP_FREQUENCY_HZ; // ms
const uint16_t TELEMETRY_PERIOD_MS = 500; // ms
const uint16_t COMMAND_TIMEOUT_MS = 300; // ms

const int16_t CONTROL_INPUT_MAX = 255; // Expected max absolute input for vx/vy/wz.

const bool ENABLE_DEBUG_SERIAL = true;

const float WHEEL_RADIUS_MM = 31.0f;
const float WHEELBASE_MM = 145.0f;
const float TRACK_WIDTH_MM = 135.0f;

// Scale normalized rotation input so wz=1.0 does not instantly saturate wheels.
const float ROTATION_SCALE = WHEEL_RADIUS_MM / (WHEELBASE_MM + TRACK_WIDTH_MM);

const uint16_t PWM_RANGE = 255;
const uint32_t PWM_FREQUENCY_HZ = 20000;

// Control-axis tuning
const bool INVERT_VY = false; // Flip strafe direction if left/right is reversed.
const bool INVERT_VY_RIGHT = true; // Flip strafe contribution on right-side wheels.

const uint8_t M1_IN1_PIN = 18; // Front left
const uint8_t M1_IN2_PIN = 19;
const uint8_t M2_IN1_PIN = 20; // Back left
const uint8_t M2_IN2_PIN = 21;
const uint8_t M3_IN1_PIN = 6;  // Front right
const uint8_t M3_IN2_PIN = 7;
const uint8_t M4_IN1_PIN = 8;  // Back right
const uint8_t M4_IN2_PIN = 9;

const bool M1_INVERT = true;
const bool M2_INVERT = false;
const bool M3_INVERT = false;
const bool M4_INVERT = false;

inline WiFiConfig getWiFiConfig() {
    WiFiConfig config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASSWORD,
        .listen_port = LISTEN_PORT,
        .telemetry_port = TELEMETRY_PORT
    };
    return config;
}

#endif // CONFIG_H