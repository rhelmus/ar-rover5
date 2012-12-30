#include "constants.h"
#include "encoders.h"
#include "motors.h"

#include <Arduino.h>

CMotors motors;


void CMotors::setEffMotorSpeed(EMotor m, uint8_t s)
{
    switch (m)
    {
    case MOTOR_LB: analogWrite(PIN_MOTOR_LB_PWM, s); break;
    case MOTOR_LF: analogWrite(PIN_MOTOR_LF_PWM, s); break;
    case MOTOR_RB: analogWrite(PIN_MOTOR_RB_PWM, s); break;
    case MOTOR_RF: analogWrite(PIN_MOTOR_RF_PWM, s); break;
    }

    motorData[m].setPower = s;
}

void CMotors::setEffMotorDirection(EMotor m, EMotorDirection d)
{
    // Note: pin direction of back/forward motors reversed
    switch (m)
    {
    case MOTOR_LB:
        digitalWrite(PIN_MOTOR_LB_DIR, (d == DIR_FWD));
        break;
    case MOTOR_LF:
        digitalWrite(PIN_MOTOR_LF_DIR, (d != DIR_FWD));
        break;
    case MOTOR_RB:
        digitalWrite(PIN_MOTOR_RB_DIR, (d == DIR_FWD));
        break;
    case MOTOR_RF:
        digitalWrite(PIN_MOTOR_RF_DIR, (d != DIR_FWD));
        break;
    }

    motorData[m].setDirection = d;
}

void CMotors::init()
{
    pinMode(PIN_MOTOR_LB_PWM, OUTPUT);
    pinMode(PIN_MOTOR_LB_DIR, OUTPUT);
    pinMode(PIN_MOTOR_LF_PWM, OUTPUT);
    pinMode(PIN_MOTOR_LF_DIR, OUTPUT);
    pinMode(PIN_MOTOR_RB_PWM, OUTPUT);
    pinMode(PIN_MOTOR_RB_DIR, OUTPUT);
    pinMode(PIN_MOTOR_RF_PWM, OUTPUT);
    pinMode(PIN_MOTOR_RF_DIR, OUTPUT);

    for (uint8_t m=0; m<MOTOR_END; ++m)
        setEffMotorDirection(static_cast<EMotor>(m), DIR_FWD);

    enabled = false;
    fixedTurning = false;
}

void CMotors::enable()
{
    digitalWrite(PIN_RELAY, HIGH);
    enabled = true;
    delay(100); // Settle a bit
}

void CMotors::disable()
{
    directStop();
    digitalWrite(PIN_RELAY, LOW);
    enabled = false;
}

void CMotors::setMotorSpeed(EMotor m, uint8_t s)
{
    if (s < MIN_POWER)
    {
        motorData[m].targetPower = 0;
        motorData[m].targetEncSpeed = 0;
    }
    else
    {
        motorData[m].targetPower = min(s, MAX_POWER);

        // Rough linear correlation between PWM and encoder speed
        motorData[m].targetEncSpeed = motorData[m].targetPower / 2 + 14;
    }
}

void CMotors::setMotorDirection(EMotor m, EMotorDirection d)
{
    motorData[m].targetDirection = d;
}

void CMotors::turn(uint8_t s, ETurnDirection d)
{
    setLeftDirection((d == DIR_LEFT) ? DIR_BWD : DIR_FWD);
    setRightDirection((d == DIR_RIGHT) ? DIR_BWD : DIR_FWD);
    setLeftSpeed(s);
    setRightSpeed(s);
}

void CMotors::moveDist(uint8_t s, uint32_t dist, EMotorDirection dir)
{
    for (uint8_t m=0; m<MOTOR_END; ++m)
    {
        motorData[m].targetDistance = dist;
        setMotorDirection(static_cast<EMotor>(m), dir);
        setMotorSpeed(static_cast<EMotor>(m), s);
    }
}

void CMotors::turnDist(uint8_t s, uint32_t dist, ETurnDirection dir)
{
    for (uint8_t m=0; m<MOTOR_END; ++m)
    {
        motorData[m].targetDistance = dist;

        if ((m == MOTOR_LB) || (m == MOTOR_LF))
            setMotorDirection(static_cast<EMotor>(m),
                              (dir == DIR_LEFT) ? DIR_BWD : DIR_FWD);
        else
            setMotorDirection(static_cast<EMotor>(m),
                              (dir == DIR_LEFT) ? DIR_FWD : DIR_BWD);

        setMotorSpeed(static_cast<EMotor>(m), s);
    }

    fixedTurning = true;
}

uint16_t CMotors::getCurrent(EMotor m) const
{
    switch (m)
    {
    case MOTOR_LB: return analogRead(PIN_MOTOR_LB_CUR);
    case MOTOR_LF: return analogRead(PIN_MOTOR_LF_CUR);
    case MOTOR_RB: return analogRead(PIN_MOTOR_RB_CUR);
    case MOTOR_RF: return analogRead(PIN_MOTOR_RF_CUR);
    }
}

bool CMotors::finishedMoving() const
{
    for (uint8_t m=0; m<MOTOR_END; ++m)
    {
        if (motorData[m].targetDistance)
            return false;
    }

    return true;
}

void CMotors::update()
{
    if (!enabled)
        return;

    if (!finishedMoving())
    {
#ifdef TRACKED_MOVEMENT
        // Assume front & back motors are/want the same

        // Left
        if (motorData[MOTOR_LB].targetDistance)
        {
            const uint32_t dist = (encoders.getDist(ENC_LB) + encoders.getDist(ENC_LF)) / 2;

            if (dist >= (motorData[MOTOR_LB].targetDistance - motorData[MOTOR_LB].targetEncSpeed))
            {
                setLeftSpeed(0);
                motorData[MOTOR_LB].targetDistance =
                        motorData[MOTOR_LF].targetDistance = 0;
            }
            else if (!fixedTurning &&
                     dist >= (motorData[MOTOR_LB].targetDistance - (motorData[MOTOR_LB].targetEncSpeed*3)))
                setLeftSpeed(max(motorData[MOTOR_LB].targetPower/2, MIN_POWER));
        }

        // Right
        if (motorData[MOTOR_RB].targetDistance)
        {
            const uint32_t dist = (encoders.getDist(ENC_RB) + encoders.getDist(ENC_RF)) / 2;

            if (dist >= (motorData[MOTOR_RB].targetDistance - motorData[MOTOR_RB].targetEncSpeed))
            {
                setRightSpeed(0);
                motorData[MOTOR_RB].targetDistance =
                        motorData[MOTOR_RF].targetDistance = 0;
            }
            else if (!fixedTurning &&
                     dist >= (motorData[MOTOR_RB].targetDistance - (motorData[MOTOR_RB].targetEncSpeed*3)))
                setRightSpeed(max(motorData[MOTOR_RB].targetPower/2, MIN_POWER));
        }

        if (finishedMoving())
            fixedTurning = false;
#else
#error "No support for non tracked movement yet"
#endif
    }

    for (uint8_t m=0; m<MOTOR_END; ++m)
    {
        if (motorData[m].targetDirection != motorData[m].setDirection)
        {
            if (motorData[m].setPower == 0)
                setEffMotorDirection(static_cast<EMotor>(m),
                                     motorData[m].targetDirection);
            else if (motorData[m].setPower < MIN_POWER)
                setEffMotorSpeed(static_cast<EMotor>(m), 0);
            else
                setEffMotorSpeed(static_cast<EMotor>(m),
                                 motorData[m].setPower - MOTOR_CHANGE_INTERVAL);
        }
        else
        {
#if 0
            if (motorData[m].targetDistance)
            {
                const uint32_t dist = encoders.getDist(static_cast<EEncoder>(m));

                if (dist >= (motorData[m].targetDistance - motorData[m].targetEncSpeed))
                {
                    motorData[m].targetPower = 0;
                    motorData[m].targetEncSpeed = 0;
                    motorData[m].targetDistance = 0;
                }
                else if (dist >= (motorData[m].targetDistance - (motorData[m].targetEncSpeed*3)))
                    setMotorSpeed(static_cast<EMotor>(m),
                                  max(motorData[m].targetPower/2, MIN_POWER));
            }
#endif

            if ((motorData[m].targetPower == motorData[m].setPower) &&
                (motorData[m].targetEncSpeed > 0))
            {
                const uint16_t espeed =
                        abs(encoders.getSpeed(static_cast<EEncoder>(m)));
                const int16_t diff = espeed - motorData[m].targetEncSpeed;

                if (abs(diff) >= 4)
                    motorData[m].targetPower -= (diff / 4);
                else if (diff != 0)
                    motorData[m].targetPower += (diff < 0);

                /*
                if (diff > 0)
                    --motorData[m].targetPower;
                else if (diff < 0)
                    ++motorData[m].targetPower;*/

                motorData[m].targetPower = constrain(motorData[m].targetPower, MIN_POWER,
                                                     MAX_POWER);
            }

            if (motorData[m].targetPower != motorData[m].setPower)
            {
                const int16_t diff = motorData[m].targetPower - motorData[m].setPower;

                if (abs(diff) <= MOTOR_CHANGE_INTERVAL)
                    setEffMotorSpeed(static_cast<EMotor>(m),
                                     motorData[m].targetPower);
                else
                    setEffMotorSpeed(static_cast<EMotor>(m),
                                     motorData[m].setPower + (diff / 2));
/*                else if (diff > 0)
                    setEffMotorSpeed(static_cast<EMotor>(m),
                                     motorData[m].setPower - MOTOR_CHANGE_INTERVAL);
                else
                    setEffMotorSpeed(static_cast<EMotor>(m),
                                     max(MIN_POWER,
                                         motorData[m].setPower + MOTOR_CHANGE_INTERVAL));*/
            }
        }
    }
}
