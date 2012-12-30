#ifndef MOTORS_H
#define MOTORS_H

#include "../../shared/shared.h"
#include "constants.h"

#include <stdint.h>

enum EMotorDirection { DIR_FWD, DIR_BWD };
enum ETurnDirection { DIR_LEFT, DIR_RIGHT };

class CMotors
{
    enum
    {
        MOTOR_CHANGE_INTERVAL = 10,
        MIN_POWER = 50,
        MAX_POWER = 160
    };

    struct SMotor
    {
        uint8_t targetPower, setPower;
        uint16_t targetEncSpeed;
        uint32_t targetDistance;
        EMotorDirection targetDirection, setDirection;

        SMotor(void)
            : targetPower(0), setPower(0), targetEncSpeed(0), targetDistance(0),
              targetDirection(DIR_FWD), setDirection(DIR_FWD) { }
    };

    volatile SMotor motorData[MOTOR_END];
    volatile bool enabled, fixedTurning;

    void setEffMotorSpeed(EMotor m, uint8_t s);
    void setEffMotorDirection(EMotor m, EMotorDirection d);

public:
    void init(void);
    void enable(void);
    void disable(void);
    bool isEnabled(void) const { return enabled; }

    void setMotorSpeed(EMotor m, uint8_t s);
    void setLeftSpeed(uint8_t s)
    { setMotorSpeed(MOTOR_LB, s); setMotorSpeed(MOTOR_LF, s); }
    void setRightSpeed(uint8_t s)
    { setMotorSpeed(MOTOR_RB, s); setMotorSpeed(MOTOR_RF, s); }

    void setMotorDirection(EMotor m, EMotorDirection d);
    void setLeftDirection(EMotorDirection d)
    { setMotorDirection(MOTOR_LB, d); setMotorDirection(MOTOR_LF, d); }
    void setRightDirection(EMotorDirection d)
    { setMotorDirection(MOTOR_RB, d); setMotorDirection(MOTOR_RF, d); }

    void move(uint8_t s, EMotorDirection d)
    { setLeftSpeed(s); setLeftDirection(d); setRightSpeed(s); setRightDirection(d); }
    void turn(uint8_t s, ETurnDirection d);
    void moveDist(uint8_t s, uint32_t dist, EMotorDirection dir);
    void moveDistCm(uint8_t s, uint16_t cm, EMotorDirection dir)
    { moveDist(s, cm * ENC_PULSES_CM, dir); }
    void turnDist(uint8_t s, uint32_t dist, ETurnDirection dir);
    void turnAngle(uint8_t s, uint16_t a, ETurnDirection d)
    { turnDist(s, a * ENC_PULSES_DEG, d); }
    void stop(void) { setLeftSpeed(0); setRightSpeed(0); }
    void directStop(void)
    { setEffMotorSpeed(MOTOR_LB, 0); setEffMotorSpeed(MOTOR_LF, 0);
      setEffMotorSpeed(MOTOR_RB, 0); setEffMotorSpeed(MOTOR_RF, 0); }

    uint16_t getCurrent(EMotor m) const;
    uint8_t getTargetPower(EMotor m) const { return motorData[m].targetPower; }
    uint8_t getSetPower(EMotor m) const { return motorData[m].setPower; }
    uint16_t getTargetSpeed(EMotor m) const { return motorData[m].targetEncSpeed; }
    uint32_t getTargetDistance(EMotor m) const { return motorData[m].targetDistance; }
    bool finishedMoving(void) const;

    void update(void);
};

extern CMotors motors;

#endif // MOTORS_H
