#ifndef UDP_HANDLER_H
#define UDP_HANDLER_H

#include <WiFiUdp.h>

#include "config/wifi_config.h"

class UDPHandler
{
public:
    UDPHandler();

    bool initialize(const WiFiConfig& config);

    String getLocalIP() const;
    
    bool receiveCommand(uint8_t* buffer, size_t buffer_size, size_t& bytes_received);

    bool sendTelemetry(const uint8_t* data, size_t data_size);

    bool isConnected() const;

    int32_t getSignalStrength() const;

private:
    WiFiUDP udp_socket_;
    uint16_t listen_port_;
    uint16_t telemetry_port_;
    IPAddress remote_ip_;
    uint16_t remote_port_;
    bool remote_address_learned_;

    bool connectToWiFi(const char* ssid, const char* password, uint32_t timeout_ms);
};

#endif // UDP_HANDLER_H