#ifndef REMOTEINTERFACE_H
#define REMOTEINTERFACE_H

#include <stdint.h>

class CRemoteInterface
{
    bool allowRemoteControl;
    uint32_t pingTime, reqUpdateDelay, statusSendDelay;

public:
    CRemoteInterface(void) : allowRemoteControl(false), pingTime(0), reqUpdateDelay(0) { }

    void init(void);
    void setRemoteControlAllowed(bool r) { allowRemoteControl = r; }
    void update(void);
};

extern CRemoteInterface remoteInterface;

#endif // REMOTEINTERFACE_H
