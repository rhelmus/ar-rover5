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
        motors.move(CRUISE_SPEED, MDIR_FWD);
        resetSharpIRSensors();
    }
    else if (s == STATE_INITTURN)
        motors.stop();
    else if (s == STATE_TURNING)
    {
        if (turnAngle > 180)
            motors.turnAngle(TURN_SPEED, 360 - turnAngle, TDIR_LEFT);
        else
            motors.turnAngle(TURN_SPEED, turnAngle, TDIR_RIGHT);
    }
}

void CRobotAI::init()
{
    setState(STATE_CRUISE);
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
            if ((sharpIRFoundHit(SHARPIR_TURRET) && (sharpIR[SHARPIR_TURRET].getAvgDist() < 40)) ||
                (sharpIRFoundHit(SHARPIR_FW) && (sharpIR[SHARPIR_FW].getAvgDist() < 40)))
            {
                turnAngle = 180; // UNDONE
                setState(STATE_INITTURN);
            }

            resetSharpIRSensors();
        }
    }
    else if (state == STATE_INITTURN)
    {
        if (!encoders.isMoving())
            setState(STATE_TURNING);
    }
    else if (state == STATE_TURNING)
    {
        if (motors.distanceReached())
            setState(STATE_CRUISE);
    }
}
