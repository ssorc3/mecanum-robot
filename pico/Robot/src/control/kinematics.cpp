#include "kinematics.h"

#include <Arduino.h>
#include <math.h>

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

static void setMotor(int in1_pin, int in2_pin, int pwm_value) {
    int value = pwm_value;
    if (value < 0) {
        value = -value;
    }
    if (value > static_cast<int>(PWM_RANGE)) {
        value = PWM_RANGE;
    }

    if (pwm_value >= 0) {
        analogWrite(in1_pin, value);
        analogWrite(in2_pin, 0);
    } else {
        analogWrite(in1_pin, 0);
        analogWrite(in2_pin, value);
    }
}

void initMotors() {
    analogWriteRange(PWM_RANGE);
    analogWriteFreq(PWM_FREQUENCY_HZ);

    pinMode(M1_IN1_PIN, OUTPUT);
    pinMode(M1_IN2_PIN, OUTPUT);
    pinMode(M2_IN1_PIN, OUTPUT);
    pinMode(M2_IN2_PIN, OUTPUT);
    pinMode(M3_IN1_PIN, OUTPUT);
    pinMode(M3_IN2_PIN, OUTPUT);
    pinMode(M4_IN1_PIN, OUTPUT);
    pinMode(M4_IN2_PIN, OUTPUT);
}

void applyMecanum(float vx, float vy, float wz) {
    float vx_n = clampAbs(vx, 1.0f);
    float vy_n = clampAbs(vy, 1.0f);
    float wz_n = clampAbs(wz, 1.0f);

    float wz_ccw = -wz_n; // Protocol uses clockwise positive.
    float k = (WHEELBASE_MM + TRACK_WIDTH_MM) / WHEEL_RADIUS_MM;

    float w_fl = vx_n + vy_n - (k * wz_ccw);
    float w_bl = vx_n - vy_n - (k * wz_ccw);
    float w_fr = vx_n - vy_n + (k * wz_ccw);
    float w_br = vx_n + vy_n + (k * wz_ccw);

    float max_abs = fabsf(w_fl);
    if (fabsf(w_bl) > max_abs) max_abs = fabsf(w_bl);
    if (fabsf(w_fr) > max_abs) max_abs = fabsf(w_fr);
    if (fabsf(w_br) > max_abs) max_abs = fabsf(w_br);

    if (max_abs > 1.0f) {
        w_fl /= max_abs;
        w_bl /= max_abs;
        w_fr /= max_abs;
        w_br /= max_abs;
    }

    int pwm_fl = static_cast<int>(w_fl * PWM_RANGE);
    int pwm_bl = static_cast<int>(w_bl * PWM_RANGE);
    int pwm_fr = static_cast<int>(w_fr * PWM_RANGE);
    int pwm_br = static_cast<int>(w_br * PWM_RANGE);

    if (ENABLE_DEBUG_SERIAL) {
        static uint32_t last_debug_ms = 0;
        uint32_t now = millis();
        if (now - last_debug_ms >= 200) {
            last_debug_ms = now;
            Serial.print("[MOTOR] fl=");
            Serial.print(pwm_fl);
            Serial.print(" bl=");
            Serial.print(pwm_bl);
            Serial.print(" fr=");
            Serial.print(pwm_fr);
            Serial.print(" br=");
            Serial.println(pwm_br);
        }
    }
    
    setMotor(M1_IN1_PIN, M1_IN2_PIN, pwm_fl);
    setMotor(M2_IN1_PIN, M2_IN2_PIN, pwm_bl);
    setMotor(M3_IN1_PIN, M3_IN2_PIN, pwm_fr);
    setMotor(M4_IN1_PIN, M4_IN2_PIN, pwm_br);
}
