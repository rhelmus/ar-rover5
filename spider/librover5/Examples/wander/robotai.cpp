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
        if (!sharpIRFoundHit(SHARPIR_LEFT, 50) && !sharpIRFoundHit(/*SHARPIR_LEFT_FW*/SHARPIR_RIGHT_FW, 50))
            return random(270, 316);
    }

    if (!sharpIRFoundHit(SHARPIR_RIGHT, 50) && !sharpIRFoundHit(/*SHARPIR_RIGHT_FW*/SHARPIR_LEFT_FW, 50))
        return random(45, 91);

    if (!startleft)
    {
        if (!sharpIRFoundHit(SHARPIR_LEFT, 50) && !sharpIRFoundHit(/*SHARPIR_LEFT_FW*/SHARPIR_RIGHT_FW, 50))
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
        if (sharpIR[SHARPIR_TURRET].getTotReadingCount() >= 8)
        {
            if (sharpIRFoundHit(SHARPIR_TURRET, 25) || sharpIRFoundHit(SHARPIR_FW, 25) ||
                sharpIRFoundHit(SHARPIR_LEFT_FW, 20) || sharpIRFoundHit(SHARPIR_RIGHT_FW, 20))
            {
                if (sharpIRFoundHit(SHARPIR_TURRET, 45))
                    digitalWrite(PIN_LED_RED_BACK, HIGH);
                else if (sharpIRFoundHit(SHARPIR_FW, 30))
                    digitalWrite(PIN_LED_RED_FRONT, HIGH);
                else if (sharpIRFoundHit(SHARPIR_LEFT_FW, 20))
                    digitalWrite(PIN_LED_GREEN_LEFT, HIGH);
                else if (sharpIRFoundHit(SHARPIR_RIGHT_FW, 20))
                    digitalWrite(PIN_LED_GREEN_RIGHT, HIGH);

                motors.moveDistCm(CRUISE_SPEED - 20, 15, MDIR_BWD);
                setState(STATE_BACKOFF);
            }
            else if (sharpIRFoundHit(SHARPIR_LEFT, 25) || /*sharpIRFoundHit(SHARPIR_LEFT_FW, 20)*/
                     sharpIRFoundHit(SHARPIR_RIGHT_FW, 40))
            {
                motors.setLeftSpeed(CRUISE_SPEED + 15);
                motors.setRightSpeed(0/*CRUISE_SPEED - 40*/);
                digitalWrite(PIN_LED_YELLOW_LEFT, HIGH);
                resetDriveTime = curtime + 750;
            }
            else if (sharpIRFoundHit(SHARPIR_RIGHT, 25) || /*sharpIRFoundHit(SHARPIR_RIGHT_FW, 20)*/
                     sharpIRFoundHit(SHARPIR_LEFT_FW, 40))
            {
                motors.setLeftSpeed(/*CRUISE_SPEED - 40*/0);
                motors.setRightSpeed(CRUISE_SPEED + 15);
                digitalWrite(PIN_LED_YELLOW_RIGHT, HIGH);
                resetDriveTime = curtime + 750;
            }
            else if (sharpIRFoundHit(SHARPIR_LEFT, 35))
            {
                motors.setLeftSpeed(CRUISE_SPEED + 15);
                motors.setRightSpeed(MIN_MOTOR_POWER);
                digitalWrite(PIN_LED_YELLOW_LEFT, HIGH);
                resetDriveTime = curtime + 750;
            }
            else if (sharpIRFoundHit(SHARPIR_RIGHT, 35))
            {
                motors.setLeftSpeed(MIN_MOTOR_POWER);
                motors.setRightSpeed(CRUISE_SPEED + 15);
                digitalWrite(PIN_LED_YELLOW_RIGHT, HIGH);
                resetDriveTime = curtime + 750;
            }
            else if (resetDriveTime && (resetDriveTime < millis()))
            {
                motors.setLeftSpeed(CRUISE_SPEED);
                motors.setRightSpeed(CRUISE_SPEED);
                digitalWrite(PIN_LED_YELLOW_LEFT, LOW);
                digitalWrite(PIN_LED_YELLOW_RIGHT, LOW);
                resetDriveTime = 0;
            }

            resetSharpIRSensors();
        }
    }
    else if (state == STATE_BACKOFF)
    {
        if (motors.distanceReached())
        {
            turnAngle = findTurnAngle();
            setState(STATE_INITTURN);
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
        {
            setState(STATE_CRUISE);
            for (uint8_t i=PIN_LED_GREEN_LEFT; i<=PIN_LED_RED_BACK; ++i)
                digitalWrite(i, LOW);
        }
    }
}
