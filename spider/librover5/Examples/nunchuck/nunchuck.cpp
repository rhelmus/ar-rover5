#include <librover5.h>

#include "nunchuck_funcs.h"

#include <Wire.h>
#include <LSM303.h>
#include <Servo.h>
#include <TimerOne.h>

namespace {

enum EJoyDir
{
    JOY_LEFT = 0,
    JOY_RIGHT,
    JOY_TOP,
    JOY_BOTTOM,
    JOY_LEFT_TOP,
    JOY_LEFT_BOTTOM,
    JOY_RIGHT_TOP,
    JOY_RIGHT_BOTTOM,
    JOY_NONE
};

enum { JOY_OFFSET_MIN = 70, JOY_OFFSET_MAX = 190 };

EJoyDir joyDir(uint16_t jx, uint16_t jy)
{
    if (jx < JOY_OFFSET_MIN) // Left?
    {
        if (jy <= JOY_OFFSET_MIN)
            return JOY_LEFT_BOTTOM;
        else if (jy >= JOY_OFFSET_MAX)
            return JOY_LEFT_TOP;
        else
            return JOY_LEFT;
    }
    else if (jx >= JOY_OFFSET_MAX) // Right?
    {
        if (jy <= JOY_OFFSET_MIN)
            return JOY_RIGHT_BOTTOM;
        else if (jy >= JOY_OFFSET_MAX)
            return JOY_RIGHT_TOP;
        else
            return JOY_RIGHT;
    }
    else if (jy <= JOY_OFFSET_MIN)
        return JOY_BOTTOM;
    else if (jy >= JOY_OFFSET_MAX)
        return JOY_TOP;
    else
        return JOY_NONE;
}

void updateMotors(uint8_t speed, EJoyDir dir)
{
    switch (dir)
    {
    case JOY_TOP: motors.move(speed, MDIR_FWD); break;
    case JOY_BOTTOM: motors.move(speed, MDIR_BWD); break;
#ifdef MECANUM_MOVEMENT
    case JOY_LEFT:
        motors.setMotorDirection(MOTOR_LF, MDIR_BWD);
        motors.setMotorDirection(MOTOR_LB, MDIR_FWD);
        motors.setMotorDirection(MOTOR_RF, MDIR_FWD);
        motors.setMotorDirection(MOTOR_RB, MDIR_BWD);
        motors.setLeftSpeed(speed);
        motors.setRightSpeed(speed);
        break;
    case JOY_RIGHT:
        motors.setMotorDirection(MOTOR_LF, MDIR_FWD);
        motors.setMotorDirection(MOTOR_LB, MDIR_BWD);
        motors.setMotorDirection(MOTOR_RF, MDIR_BWD);
        motors.setMotorDirection(MOTOR_RB, MDIR_FWD);
        motors.setLeftSpeed(speed);
        motors.setRightSpeed(speed);
        break;
    case JOY_LEFT_TOP:
        motors.setMotorDirection(MOTOR_LB, MDIR_BWD);
        motors.setMotorDirection(MOTOR_RF, MDIR_BWD);
        motors.setMotorSpeed(MOTOR_LF, 0);
        motors.setMotorSpeed(MOTOR_LB, speed);
        motors.setMotorSpeed(MOTOR_RF, speed);
        motors.setMotorSpeed(MOTOR_RB, 0);
        break;
    case JOY_LEFT_BOTTOM:
        motors.setMotorDirection(MOTOR_LF, MDIR_FWD);
        motors.setMotorDirection(MOTOR_RB, MDIR_FWD);
        motors.setMotorSpeed(MOTOR_LF, speed);
        motors.setMotorSpeed(MOTOR_LB, 0);
        motors.setMotorSpeed(MOTOR_RF, 0);
        motors.setMotorSpeed(MOTOR_RB, speed);
        break;
    case JOY_RIGHT_TOP:
        motors.setMotorDirection(MOTOR_LF, MDIR_BWD);
        motors.setMotorDirection(MOTOR_RB, MDIR_BWD);
        motors.setMotorSpeed(MOTOR_LF, speed);
        motors.setMotorSpeed(MOTOR_LB, 0);
        motors.setMotorSpeed(MOTOR_RF, 0);
        motors.setMotorSpeed(MOTOR_RB, speed);
        break;
    case JOY_RIGHT_BOTTOM:
        motors.setMotorDirection(MOTOR_LB, MDIR_FWD);
        motors.setMotorDirection(MOTOR_RF, MDIR_FWD);
        motors.setMotorSpeed(MOTOR_LF, 0);
        motors.setMotorSpeed(MOTOR_LB, speed);
        motors.setMotorSpeed(MOTOR_RF, speed);
        motors.setMotorSpeed(MOTOR_RB, 0);
        break;
#else
    case JOY_LEFT: motors.turn(speed, DIR_LEFT); break;
    case JOY_RIGHT: motors.turn(speed, DIR_RIGHT); break;
    case JOY_LEFT_TOP:
        motors.setLeftSpeed(0);
        motors.setRightSpeed(speed);
        motors.setRightDirection(MDIR_FWD);
        break;
    case JOY_LEFT_BOTTOM:
        motors.setLeftSpeed(0);
        motors.setRightSpeed(speed);
        motors.setRightDirection(MDIR_BWD);
        break;
    case JOY_RIGHT_TOP:
        motors.setLeftSpeed(speed);
        motors.setLeftDirection(MDIR_FWD);
        motors.setRightSpeed(0);
        break;
    case JOY_RIGHT_BOTTOM:
        motors.setLeftSpeed(speed);
        motors.setLeftDirection(MDIR_BWD);
        motors.setRightSpeed(0);
        break;
#endif
    case JOY_NONE: motors.stop(); break;
    }
}

}


void setup()
{
    Serial.begin(115200);

    initRover5();
    motors.enable();

    nunchuck_init();
    nunchuck_get_data(); // First data is bogus

    Serial.println("Initialized");
}

void loop()
{
    static uint32_t updelay;
    static EJoyDir lastjdir = JOY_NONE;
    static uint8_t speed = 70;
    const uint32_t curtime = millis();

    if (curtime > updelay)
    {
        updelay = millis() + 250;

        nunchuck_get_data();

        const uint8_t cbut = nunchuck_cbutton(), zbut = nunchuck_zbutton();

        if (cbut && zbut)
            speed = 70; // reset
        else if (cbut)
            speed = min(speed+5, MAX_MOTOR_POWER);
        else if (zbut)
            speed = max(speed-5, MIN_MOTOR_POWER);

        const uint16_t joyx = nunchuck_joyx();
        const uint16_t joyy = nunchuck_joyy();

//        Serial.print("Joy dir: ");
        const EJoyDir dir = joyDir(joyx, joyy);

        if (cbut || zbut || (dir != lastjdir))
        {
            Serial.print("speed: "); Serial.println(speed, DEC);
            Serial.print("dir: "); Serial.println(dir, DEC);
            lastjdir = dir;
            updateMotors(speed, dir);
        }      


//        switch (dir)
//        {
//        case JOY_LEFT: Serial.println("left"); break;
//        case JOY_RIGHT: Serial.println("right"); break;
//        case JOY_TOP: Serial.println("top"); break;
//        case JOY_BOTTOM: Serial.println("bottom"); break;
//        case JOY_LEFT_TOP: Serial.println("left-top"); break;
//        case JOY_LEFT_BOTTOM: Serial.println("left-bottom"); break;
//        case JOY_RIGHT_TOP: Serial.println("right-top"); break;
//        case JOY_RIGHT_BOTTOM: Serial.println("right-bottom"); break;
//        }

//        Serial.print("  joyx: "); Serial.print(joyx, DEC);
//        Serial.print("  joyy: "); Serial.println(joyy, DEC);
    }

    rover5Task();
}
