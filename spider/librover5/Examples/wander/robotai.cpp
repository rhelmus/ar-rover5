#include "../../shared/shared.h"
#include "robotai.h"

#include <librover5.h>

namespace {

bool sharpIRFoundHit(ESharpIRSensor s, uint8_t dist)
{
    return ((sharpIR[s].getHitCount() >= (sharpIR[s].getTotReadingCount() * 0.75)) &&
            (sharpIR[s].getAvgDist() < dist));
}

uint16_t findTurnAngle(void)
{
    const bool startleft = random(0, 2);

    if (startleft)
    {
        if (!sharpIRFoundHit(SHARPIR_LEFT, 50) && !sharpIRFoundHit(SHARPIR_LEFT_FW, 50))
            return random(270, 316);
    }

    if (!sharpIRFoundHit(SHARPIR_RIGHT, 50) && !sharpIRFoundHit(SHARPIR_RIGHT_FW, 50))
        return random(45, 91);

    if (!startleft)
    {
        if (!sharpIRFoundHit(SHARPIR_LEFT, 50) && !sharpIRFoundHit(SHARPIR_LEFT_FW, 50))
            return random(270, 316);
    }

    return random(135, 226);
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
    setState(STATE_NONE);
}

void CRobotAI::think()
{
    const uint32_t curtime = millis();

    if (checkLDRTime < curtime)
    {
        checkLDRTime = curtime + 1000;
        digitalWrite(PIN_FRONTLEDS, (analogRead(PIN_LDR) < 500) ? HIGH : LOW);
    }

    if (state == STATE_CRUISE)
    {
        // NOTE: Assume that every sharp sensor has the same reading count
        if (sharpIR[SHARPIR_TURRET].getTotReadingCount() >= 3)
        {
            if (sharpIRFoundHit(SHARPIR_TURRET, 40) || sharpIRFoundHit(SHARPIR_FW, 40))
            {
                turnAngle = findTurnAngle();
                setState(STATE_INITTURN);
            }
            else if (sharpIRFoundHit(SHARPIR_LEFT, 15) || sharpIRFoundHit(SHARPIR_LEFT_FW, 20))
            {
                motors.setLeftSpeed(CRUISE_SPEED + 15);
                motors.setRightSpeed(CRUISE_SPEED - 40);
                resetDriveTime = curtime + 750;
            }
            else if (sharpIRFoundHit(SHARPIR_RIGHT, 15) || sharpIRFoundHit(SHARPIR_RIGHT_FW, 20))
            {
                motors.setLeftSpeed(CRUISE_SPEED - 40);
                motors.setRightSpeed(CRUISE_SPEED + 15);
                resetDriveTime = curtime + 750;
            }
            else if (resetDriveTime && (resetDriveTime < millis()))
            {
                motors.setLeftSpeed(CRUISE_SPEED);
                motors.setRightSpeed(CRUISE_SPEED);
                resetDriveTime = 0;
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
        if (motors.distanceReached() && !encoders.isMoving())
            setState(STATE_CRUISE);
    }
}
