#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "config/config.h"
#include "control/kinematics.h"
#include "network/udp_handler.h"
#include "protocol/control_protocol.h"
#include "telemetry/telemetry.h"

UDPHandler udp;
ControlCommand last_command;
uint32_t last_command_ms = 0;
uint8_t telemetry_seq = 0;
const uint32_t HEARTBEAT_PERIOD_MS = 1000;

void setup()
{
  if (ENABLE_DEBUG_SERIAL) {
    Serial.begin(115200);
  }

  delay(2000);

  if (ENABLE_DEBUG_SERIAL) {
    Serial.println("\n\n=== Mecanum Robot Control ===");
  }

  WiFiConfig wifi_config = getWiFiConfig();
  if(!udp.initialize(wifi_config)) {
    if (ENABLE_DEBUG_SERIAL) {
      Serial.println("FATAL: Failed to initialize UDP handler");
    }
    while(1) {}
  }

  initMotors();
  resetControlCommand(last_command);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  if (ENABLE_DEBUG_SERIAL) {
    Serial.println("Setup complete!");
  }
}

void loop()
{
  uint8_t command_buffer[64];
  size_t bytes_received = 0;

  if(udp.receiveCommand(command_buffer, sizeof(command_buffer), bytes_received)) {
    ControlCommand cmd;
    uint8_t seq = 0;
    if (parseControlFrame(command_buffer, bytes_received, cmd, seq)) {
      if (ENABLE_DEBUG_SERIAL) {
        Serial.print("[CTRL] seq=");
        Serial.print(seq);
      }
      if (cmd.has_drive) {
        last_command = cmd;
        last_command_ms = millis();
        if (ENABLE_DEBUG_SERIAL) {
          Serial.print(" drive vx=");
          Serial.print(cmd.vx);
          Serial.print(" vy=");
          Serial.print(cmd.vy);
          Serial.print(" wz=");
          Serial.println(cmd.wz);
        }
      } else if (ENABLE_DEBUG_SERIAL) {
        Serial.println(" no drive TLV");
      }
    } else if (ENABLE_DEBUG_SERIAL) {
      Serial.println("[CTRL] Invalid control frame");
    }
  }

  // Control loop timing
  static unsigned long last_control_time = 0;
  static unsigned long last_telemetry_time = 0;
  static unsigned long last_heartbeat_time = 0;
  unsigned long now = millis();

  if (now - last_control_time >= CONTROL_LOOP_PERIOD_MS) {
    last_control_time = now;

    bool has_recent_command = (now - last_command_ms) <= COMMAND_TIMEOUT_MS;
    float vx = 0.0f;
    float vy = 0.0f;
    float wz = 0.0f;

    if (has_recent_command && last_command.has_drive) {
      vx = static_cast<float>(last_command.vx) / 255.0f;
      vy = static_cast<float>(last_command.vy) / 255.0f;
      wz = static_cast<float>(last_command.wz) / 255.0f;
    }

    applyMecanum(vx, vy, wz);
  }

  sendTelemetryIfDue(udp, now, TELEMETRY_PERIOD_MS, last_telemetry_time, telemetry_seq);

  if (now - last_heartbeat_time >= HEARTBEAT_PERIOD_MS) {
    last_heartbeat_time = now;
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}
