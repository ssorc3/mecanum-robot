#include "udp_handler.h"
#include <WiFi.h>

const uint32_t WIFI_CONNECT_TIMEOUT_MS = 20000;

UDPHandler::UDPHandler()
    : listen_port_(0),
      remote_address_learned_(false)
{}

bool UDPHandler::initialize(const WiFiConfig& config) {
    listen_port_ = config.listen_port;

    Serial.println("[UDP] Connecting to WiFi...");
    if(!connectToWiFi(config.ssid, config.password, WIFI_CONNECT_TIMEOUT_MS)) {
        Serial.println("[UDP] Failed to connect to WiFi");
        return false;
    }

    Serial.print("[UDP] Connected! IP: ");
    Serial.println(getLocalIP());
    Serial.print("[UDP] Signal Strength: ");
    Serial.print(getSignalStrength());
    Serial.println(" dBm");

    // Start UDP socket on listen port
    if (!udp_socket_.begin(listen_port_)) {
        Serial.print("[UDP] Failed to start UDP on port ");
        Serial.println(listen_port_);
        return false;
    }

    Serial.print("[UDP] Listening for commands on port ");
    Serial.println(listen_port_);

    return true;
}

bool UDPHandler::connectToWiFi(const char* ssid, const char* password, uint32_t timeout_ms) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    uint32_t start_time = millis();

    while(WiFi.status() != WL_CONNECTED) {
        if(millis() - start_time > timeout_ms) {
            Serial.println("[UDP] WiFi connection timeout");
            return false;
        }

        delay(500);
        Serial.print(".");
    }

    Serial.println();
    return true;
}

String UDPHandler::getLocalIP() const {
    return WiFi.localIP().toString();
}

bool UDPHandler::receiveCommand(uint8_t* buffer, size_t buffer_size, size_t& bytes_received) {
    bytes_received = 0;

    int packet_size = udp_socket_.parsePacket();
    if (packet_size <= 0) {
        return false;
    }

    size_t read_size = (packet_size < buffer_size) ? packet_size : buffer_size;

    int bytes_read = udp_socket_.read(buffer, read_size);
    if (bytes_read <= 0) {
        return false;
    }
    
    bytes_received = bytes_read;

    if(!remote_address_learned_) {
        remote_ip_ = udp_socket_.remoteIP();
        remote_port_ = udp_socket_.remotePort();
        remote_address_learned_ = true;

        Serial.print("[UDP] Learned remote address: ");
        Serial.print(remote_ip_);
        Serial.print(":");
        Serial.println(remote_port_);
    }

    return true;
}

bool UDPHandler::sendTelemetry(const uint8_t* data, size_t data_size) {
    if(!remote_address_learned_) {
        return false;
    }

    udp_socket_.beginPacket(remote_ip_, remote_port_);
    udp_socket_.write(data, data_size);
    return udp_socket_.endPacket() == 1;
}

bool UDPHandler::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

int32_t UDPHandler::getSignalStrength() const {
    return WiFi.RSSI();
}