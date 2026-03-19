#include "command_receiver.h"

#include "protocol/control_protocol.h"
#include "util/log.h"

void receiveCommands(UDPHandler& udp, DriveController& drive, uint32_t now_ms) {
    uint8_t command_buffer[64];
    size_t bytes_received = 0;

    if (!udp.receiveCommand(command_buffer, sizeof(command_buffer), bytes_received)) {
        return;
    }

    ControlCommand cmd;
    uint8_t seq = 0;
    if (parseControlFrame(command_buffer, bytes_received, cmd, seq)) {
        logPrint("[CTRL] seq=");
        logPrint(seq);
        if (cmd.has_drive) {
            drive.acceptCommand(cmd, now_ms);
            logPrint(" drive vx=");
            logPrint(cmd.vx);
            logPrint(" vy=");
            logPrint(cmd.vy);
            logPrint(" wz=");
            logPrintln(cmd.wz);
        } else {
            logPrintln(" no drive TLV");
        }
    } else {
        logPrintln("[CTRL] Invalid control frame");
    }
}
