#ifndef MOTORS_H
#define MOTORS_H

#include "../../shared/shared.h"
#include "constants.h"

#include <stdint.h>

class CMotors
{
    enum { MOTOR_CHANGE_INTERVAL = 10 };

    struct SMotor
    {
        uint8_t requestedPower, targetPower, setPower;
        uint16_t targetEncSpeed;
        uint32_t targetDistance;
        EMotorDirection targetDirection, setDirection;

        SMotor(void)
            : requestedPower(0), targetPower(0), setPower(0), targetEncSpeed(0),
              targetDistance(0), targetDirection(MDIR_FWD), setDirection(MDIR_FWD) { }
    };

    volatile SMotor motorData[MOTOR_END];
    volatile bool enabled, fixedTurning;
    volatile uint32_t endTime;

    void reset(void);
    void setEffMotorSpeed(EMotor m, uint8_t s);
    void setEffMotorDirection(EMotor m, EMotorDirection d);

public:
    CMotors(void) : enabled(false), fixedTurning(false), endTime(0) { }
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
    void moveDist(uint8_t s, uint32_t dist, EMotorDirection dir);
    void moveDistCm(uint8_t s, uint16_t cm, EMotorDirection dir)
    { moveDist(s, cm * ENC_PULSES_CM, dir); }
    void turn(uint8_t s, ETurnDirection d);
    void turnDist(uint8_t s, uint32_t dist, ETurnDirection dir);
    void turnAngle(uint8_t s, uint16_t a, ETurnDirection d)
    { turnDist(s, a * ENC_PULSES_DEG, d); }
#ifdef MECANUM_MOVEMENT
    void translateDist(uint8_t s, uint32_t dist, ETranslateDirection dir);
    void translate(uint8_t s, ETranslateDirection d) { translateDist(s, 0, d); }
    void translateCm(uint8_t s, uint16_t dist, ETranslateDirection dir)
    { translateDist(s, dist * ENC_PULSES_CM, dir); } // UNDONE: Calibrate
#endif
    void setDuration(uint32_t t);
    void stop(void);
    void directStop(void)
    { stop(); setEffMotorSpeed(MOTOR_LB, 0); setEffMotorSpeed(MOTOR_LF, 0);
      setEffMotorSpeed(MOTOR_RB, 0); setEffMotorSpeed(MOTOR_RF, 0); }

    uint16_t getCurrent(EMotor m) const;
    uint8_t getTargetPower(EMotor m) const { return motorData[m].targetPower; }
    uint8_t getSetPower(EMotor m) const { return motorData[m].setPower; }
    uint16_t getTargetSpeed(EMotor m) const { return motorData[m].targetEncSpeed; }
    uint32_t getTargetDistance(EMotor m) const { return motorData[m].targetDistance; }
    bool distanceReached(void) const;

    void update(void);
};

extern CMotors motors;

#endif // MOTORS_H
