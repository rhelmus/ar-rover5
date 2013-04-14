#ifndef ROBOTAI_H
#define ROBOTAI_H

#include <stdint.h>

class CRobotAI
{
    enum EState { STATE_CRUISE, STATE_INITTURN, STATE_TURNING };
    enum
    {
        CRUISE_SPEED = 80,
        TURN_SPEED = 60
    };

    EState state;
    uint16_t turnAngle;

    void setState(EState s);

public:
    CRobotAI(void) { }

    void init(void);
    void stop(void);
    void think(void);
};

#endif // ROBOTAI_H
