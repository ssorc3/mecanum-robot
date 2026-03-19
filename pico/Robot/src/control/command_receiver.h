#ifndef COMMAND_RECEIVER_H
#define COMMAND_RECEIVER_H

#include <stdint.h>

#include "control/drive.h"
#include "network/udp_handler.h"

void receiveCommands(UDPHandler& udp, DriveController& drive, uint32_t now_ms);

#endif // COMMAND_RECEIVER_H
