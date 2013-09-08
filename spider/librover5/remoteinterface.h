#ifndef REMOTEINTERFACE_H
#define REMOTEINTERFACE_H

#include "../../shared/shared.h"
#include "ringbuffer.h"

#include <stdint.h>

class CRemoteInterface
{
    enum { RECBUFFER_SIZE = 16 };

    bool allowRemoteControl;
    uint32_t statusSendDelay;
    bool receivedSerRecMsgStart, receivedSerRecMsgSize;
    uint8_t serRecMsgSize;
    EMessage currentSerRecMsg;
    uint8_t serRecMsgBytesRead;
    CRingBuffer<RECBUFFER_SIZE> tempSerialMsgBuffer;

    void updateMotorControl(void);
    void sendStatus(void);
    void checkForCommands(void);
    void handleCommand(EMessage cmd);

public:
    CRemoteInterface(void)
        : allowRemoteControl(false), statusSendDelay(0), receivedSerRecMsgStart(false),
          receivedSerRecMsgSize(false), serRecMsgSize(0), currentSerRecMsg(MSG_NONE),
          serRecMsgBytesRead(0) { }

    void init(void);
    void setRemoteControlAllowed(bool r) { allowRemoteControl = r; }
    void update(void);
};

extern CRemoteInterface remoteInterface;

#endif // REMOTEINTERFACE_H
