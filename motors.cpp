#include "constants.h"
#include "encoders.h"
#include "motors.h"

#include <Arduino.h>

CMotors motors;


void CMotors::setEffMotorSpeed(EMotor m, uint8_t s)
{
    switch (m)
    {
    case MOTOR_LEFT_BACK: analogWrite(PIN_MOTOR_LB_PWM, s); break;
    case MOTOR_LEFT_FRONT: analogWrite(PIN_MOTOR_LF_PWM, s); break;
    case MOTOR_RIGHT_BACK: analogWrite(PIN_MOTOR_RB_PWM, s); break;
    case MOTOR_RIGHT_FRONT: analogWrite(PIN_MOTOR_RF_PWM, s); break;
    }

    motorData[m].setPower = s;
}

void CMotors::setEffMotorDirection(EMotor m, EMotorDirection d)
{
    // Note: pin direction of back/forward motors reversed
    switch (m)
    {
    case MOTOR_LEFT_BACK:
        digitalWrite(PIN_MOTOR_LB_DIR, (d == DIR_FWD));
        break;
    case MOTOR_LEFT_FRONT:
        digitalWrite(PIN_MOTOR_LF_DIR, (d != DIR_FWD));
        break;
    case MOTOR_RIGHT_BACK:
        digitalWrite(PIN_MOTOR_RB_DIR, (d == DIR_FWD));
        break;
    case MOTOR_RIGHT_FRONT:
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

    for (uint8_t m=0; m<MOTOR_COUNT; ++m)
        setEffMotorDirection(static_cast<EMotor>(m), DIR_FWD);
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

uint16_t CMotors::getCurrent(EMotor m) const
{
    switch (m)
    {
    case MOTOR_LEFT_BACK: return analogRead(PIN_MOTOR_LB_CUR);
    case MOTOR_LEFT_FRONT: return analogRead(PIN_MOTOR_LF_CUR);
    case MOTOR_RIGHT_BACK: return analogRead(PIN_MOTOR_RB_CUR);
    case MOTOR_RIGHT_FRONT: return analogRead(PIN_MOTOR_RF_CUR);
    }
}

void CMotors::update()
{
    for (uint8_t m=0; m<MOTOR_COUNT; ++m)
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
        else if (motorData[m].targetPower != motorData[m].setPower)
        {
            const int16_t diff = motorData[m].setPower - motorData[m].targetPower;

            if (abs(diff) < MOTOR_CHANGE_INTERVAL)
                setEffMotorSpeed(static_cast<EMotor>(m),
                                 motorData[m].targetPower);
            else if (diff > 0)
                setEffMotorSpeed(static_cast<EMotor>(m),
                                 motorData[m].setPower - MOTOR_CHANGE_INTERVAL);
            else
                setEffMotorSpeed(static_cast<EMotor>(m),
                                 max(MIN_POWER,
                                     motorData[m].setPower + MOTOR_CHANGE_INTERVAL));
        }
        else if (motorData[m].targetEncSpeed > 0)
        {
            const uint16_t espeed =
                    abs(encoders.getSpeed(static_cast<EEncoder>(m)));
            const int16_t diff = espeed - motorData[m].targetEncSpeed;

            if (diff > 0)
                --motorData[m].targetPower;
            else if (diff < 0)
                ++motorData[m].targetPower;

            motorData[m].targetPower = constrain(motorData[m].targetPower, MIN_POWER,
                                                 MAX_POWER);
        }
    }
}
