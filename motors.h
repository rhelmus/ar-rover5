#ifndef MOTORS_H
#define MOTORS_H

#include <stdint.h>

enum EMotor
{
    MOTOR_LEFT_BACK=0,
    MOTOR_LEFT_FRONT,
    MOTOR_RIGHT_BACK,
    MOTOR_RIGHT_FRONT,
    MOTOR_COUNT
};

enum EMotorDirection { DIR_FWD, DIR_BWD };


class CMotors
{
    enum
    {
        MOTOR_CHANGE_INTERVAL = 10,
        MIN_POWER = 60,
        MAX_POWER = 160
    };

    struct SMotor
    {
        uint8_t targetPower, setPower;
        uint16_t targetEncSpeed;
        EMotorDirection targetDirection, setDirection;

        SMotor(void)
            : targetPower(0), setPower(0), targetEncSpeed(0), targetDirection(DIR_FWD),
              setDirection(DIR_FWD) { }
    };

    volatile SMotor motorData[MOTOR_COUNT];

    void setEffMotorSpeed(EMotor m, uint8_t s);
    void setEffMotorDirection(EMotor m, EMotorDirection d);

public:
    void init(void);

    void setMotorSpeed(EMotor m, uint8_t s);
    void setLeftSpeed(uint8_t s)
    { setMotorSpeed(MOTOR_LEFT_BACK, s); setMotorSpeed(MOTOR_LEFT_FRONT, s); }
    void setRightSpeed(uint8_t s)
    { setMotorSpeed(MOTOR_RIGHT_BACK, s); setMotorSpeed(MOTOR_RIGHT_FRONT, s); }

    void setMotorDirection(EMotor m, EMotorDirection d);
    void setLeftDirection(EMotorDirection d)
    { setMotorDirection(MOTOR_LEFT_BACK, d); setMotorDirection(MOTOR_LEFT_FRONT, d); }
    void setRightDirection(EMotorDirection d)
    { setMotorDirection(MOTOR_RIGHT_BACK, d); setMotorDirection(MOTOR_RIGHT_FRONT, d); }

    void move(uint8_t s, EMotorDirection d)
    { setLeftSpeed(s); setLeftDirection(d); setRightSpeed(s); setRightDirection(d); }
    void stop(void) { setLeftSpeed(0); setRightSpeed(0); }
    void emergencyStop(void)
    { setEffMotorSpeed(MOTOR_LEFT_BACK, 0); setEffMotorSpeed(MOTOR_LEFT_FRONT, 0);
      setEffMotorSpeed(MOTOR_RIGHT_BACK, 0); setEffMotorSpeed(MOTOR_RIGHT_FRONT, 0); }

    uint16_t getCurrent(EMotor m) const;

    void update(void);
};

extern CMotors motors;

#endif // MOTORS_H
