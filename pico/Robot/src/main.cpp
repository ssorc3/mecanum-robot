#include <Arduino.h>

#include "config/config.h"
#include "control/command_receiver.h"
#include "control/drive.h"
#include "network/udp_handler.h"
#include "telemetry/telemetry.h"
#include "util/heartbeat_led.h"
#include "util/log.h"

const uint32_t HEARTBEAT_PERIOD_MS = 1000;

UDPHandler udp;
DriveController drive(CONTROL_LOOP_PERIOD_MS);
TelemetryService telemetry(TELEMETRY_PERIOD_MS);
HeartbeatLed heartbeat(HEARTBEAT_PERIOD_MS);

void setup()
{
  initDebugSerial();

  delay(2000);

  logPrintln("\n\n=== Mecanum Robot Control ===");

  WiFiConfig wifi_config = getWiFiConfig();
  if(!udp.initialize(wifi_config)) {
    logPrintln("FATAL: Failed to initialize UDP handler");
    while(1) {}
  }

  drive.begin();
  heartbeat.begin();

  logPrintln("Setup complete!");
}

void loop()
{
  uint32_t now = millis();

  receiveCommands(udp, drive, now);
  drive.update(now);
  telemetry.update(udp, now);
  heartbeat.update(now);
}
