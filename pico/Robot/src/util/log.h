#ifndef LOG_H
#define LOG_H

#include <Arduino.h>

#include "config/config.h"

inline void initDebugSerial() {
    if (ENABLE_DEBUG_SERIAL) {
        Serial.begin(115200);
    }
}

template <typename T>
inline void logPrint(const T& value) {
    if (ENABLE_DEBUG_SERIAL) {
        Serial.print(value);
    }
}

template <typename T>
inline void logPrintln(const T& value) {
    if (ENABLE_DEBUG_SERIAL) {
        Serial.println(value);
    }
}

inline void logPrintln() {
    if (ENABLE_DEBUG_SERIAL) {
        Serial.println();
    }
}

#endif // LOG_H
