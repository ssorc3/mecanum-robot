#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <Arduino.h>

struct WiFiConfig {
    const char* ssid;
    const char* password;
    uint16_t listen_port;
    uint16_t telemetry_port;
};

#endif // WIFI_CONFIG_H