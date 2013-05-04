#ifndef ROBOTAI_H
#define ROBOTAI_H

#include <stdint.h>

class CRobotAI
{
    enum EState { STATE_CRUISE, STATE_BACKOFF, STATE_TURNING, STATE_NONE };

    EState state;
    uint32_t checkSharpIRDelay;

    void setState(EState s);

public:
    CRobotAI(void) : state(STATE_NONE) { }

    void init(void);
    void stop(void);
    void think(void);
};

#endif // ROBOTAI_H
