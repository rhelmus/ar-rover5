#include "../../shared/shared.h"
#include "robotai.h"

#include <librover5.h>

namespace {

bool sharpIRFoundHit(ESharpIRSensor s)
{
    return sharpIR[s].getHitCount() >= (sharpIR[s].getReadingCount() * 0.75);
}


}


void CRobotAI::setState(EState s)
{
    if (state == s)
        return;

    state = s;

    if (s == STATE_CRUISE)
    {
        motors.move(80, MDIR_FWD);
        resetSharpIRSensors();
    }
}

void CRobotAI::init()
{
    setState(STATE_CRUISE);
    checkSharpIRDelay = 0;
}

void CRobotAI::stop()
{
    motors.stop();
}

void CRobotAI::think()
{
    if (state == STATE_CRUISE)
    {
        // NOTE: Assume that every sharp sensor has the same reading count
        if (sharpIR[SHARPIR_TURRET].getReadingCount() >= 3)
        {
            if (sharpIRFoundHit(SHARPIR_TURRET) && (sharpIR[SHARPIR_TURRET].getAvgDist() < 40))
            {
                motors.turnAngle(80, 180, TDIR_LEFT);
                setState(STATE_TURNING);
            }

            resetSharpIRSensors();
        }
    }
    else if (state == STATE_TURNING)
    {
        if (motors.distanceReached())
            setState(STATE_CRUISE);
    }
}
