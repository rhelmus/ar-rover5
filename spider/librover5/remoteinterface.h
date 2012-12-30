#ifndef REMOTEINTERFACE_H
#define REMOTEINTERFACE_H

#include <stdint.h>

class CRemoteInterface
{
    bool allowRemoteControl;
    uint32_t pingTime, updatePingDelay;
    uint32_t statusSendDelay;

public:
    CRemoteInterface(void) : allowRemoteControl(false), pingTime(0), updatePingDelay(0) { }

    void setRemoteControlAllowed(bool r) { allowRemoteControl = r; }
    void update(void);
};

extern CRemoteInterface remoteInterface;

#endif // REMOTEINTERFACE_H
