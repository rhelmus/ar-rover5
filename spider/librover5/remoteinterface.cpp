#include "../../shared/shared.h"
#include "encoders.h"
#include "librover5.h"
#include "motors.h"
#include "remoteinterface.h"
#include "sharpir.h"

#include <Arduino.h>
#include <LSM303.h>
#include <Servo.h>
#include <Wire.h>

CRemoteInterface remoteInterface;

namespace {

struct SMotorControl
{
    bool update;
    bool turn;

    union
    {
        uint8_t leftSpeed;
        uint8_t driveSpeed;
        uint8_t turnSpeed;
    };
    uint8_t rightSpeed;

    union
    {
        EMotorDirection mdirLeft;
        EMotorDirection driveDir;
        ETurnDirection turnDir;
    };
    EMotorDirection mdirRight;

    union
    {
        uint16_t distance;
        uint16_t angle;
    };

    uint16_t duration;
};

volatile SMotorControl motorControl;


void getPitchRollHeading(int16_t &p, int16_t &r, uint16_t &h)
{
    getCompass().read();

    // From LSM303 application note
    // NOTE: pitch&roll are switched
    LSM303::vector vec = getCompass().a;
    getCompass().vector_normalize(&vec);
    const float fp = asin(-vec.x);
    r = round(fp * 180.0 / M_PI);
    p = round(asin(-vec.y / cos(fp)) * 180.0 / M_PI);

    h = getCompass().heading();
}

void TWIStartMessage(EMessage m)
{
    Wire.beginTransmission(BRIDGE_TWI_ADDRESS);
    Wire.write(m);
}

void TWIRequest(EMessage m, uint16_t n)
{
    TWIStartMessage(m);
    Wire.endTransmission();
    Wire.requestFrom(BRIDGE_TWI_ADDRESS, n);
}

void TWISendInt(uint16_t i)
{
    uint8_t buf[2];
    intToBytes(i, buf);
    Wire.write(buf, 2);
}

void TWISendLong(uint32_t i)
{
    uint8_t buf[4];
    longToBytes(i, buf);
    Wire.write(buf, 4);
}

void TWIReceiveCB(int bytes)
{
    const EMessage msg = static_cast<EMessage>(Wire.read());
    --bytes;

    if (msg == MSG_CMD_MOTORSPEED)
    {
        motorControl.update = true;
        motorControl.leftSpeed = Wire.read();
        motorControl.rightSpeed = Wire.read();
        motorControl.mdirLeft = static_cast<EMotorDirection>(Wire.read());
        motorControl.mdirRight = static_cast<EMotorDirection>(Wire.read());
        motorControl.duration = bytesToInt(Wire.read(), Wire.read());
        motorControl.distance = 0;
        motorControl.turn = false;
    }
    else if (msg == MSG_CMD_DRIVEDIST)
    {
        motorControl.update = true;
        motorControl.driveSpeed = Wire.read();
        motorControl.distance = bytesToInt(Wire.read(), Wire.read());
        motorControl.driveDir = static_cast<EMotorDirection>(Wire.read());
        motorControl.duration = 0;
        motorControl.turn = false;
    }
    else if (msg == MSG_CMD_TURN)
    {
        motorControl.update = true;
        motorControl.turnSpeed = Wire.read();
        motorControl.turnDir = static_cast<ETurnDirection>(Wire.read());
        motorControl.duration = bytesToInt(Wire.read(), Wire.read());
        motorControl.turn = true;
        motorControl.angle = 0;
    }
    else if (msg == MSG_CMD_TURNANGLE)
    {
        motorControl.update = true;
        motorControl.turnSpeed = Wire.read();
        motorControl.angle = bytesToInt(Wire.read(), Wire.read());
        motorControl.turnDir = static_cast<ETurnDirection>(Wire.read());
        motorControl.duration = 0;
        motorControl.turn = true;
    }
    else if (msg == MSG_CMD_STOP)
    {
        motorControl.update = true;
        motorControl.leftSpeed = motorControl.rightSpeed = 0;
        motorControl.turn = false;
    }
    else // Unknown message
    {
        while (bytes)
        {
            Wire.read();
            --bytes;
        }
    }
}

}


void CRemoteInterface::init()
{
    Wire.begin(SPIDER_TWI_ADDRESS);
    Wire.onReceive(TWIReceiveCB);
}

void CRemoteInterface::update()
{
    const uint32_t curtime = millis();
    if (curtime > reqUpdateDelay)
    {
        reqUpdateDelay = curtime + 100;

#if 0
        TWIRequest(MSG_REQBTDATAN, 1);
        const uint8_t msgcount = Wire.read();
        for (uint8_t i=0; i<msgcount; ++i)
        {
            const uint8_t bytes = TWIRequest(MSG_REQBTMSG, BRIDGE_MAX_REQSIZE);
            parseBTMsg(static_cast<EMessage>(Wire.read()));
        }
#endif

//        Serial.print("Pong: "); Serial.println(pingTime);
    }

    if (motorControl.update)
    {
        if (motorControl.turn)
        {
            if (!motorControl.angle)
            {
                motors.turn(motorControl.turnSpeed, motorControl.turnDir);
                if (motorControl.duration)
                    motors.setDuration(motorControl.duration * 1000);
            }
            else
                motors.turnAngle(motorControl.turnSpeed, motorControl.angle,
                                 motorControl.turnDir);
        }
        else
        {
            if (!motorControl.leftSpeed && !motorControl.rightSpeed)
                motors.stop();
            else if (!motorControl.distance)
            {
                motors.setLeftDirection(motorControl.mdirLeft);
                motors.setRightDirection(motorControl.mdirRight);
                motors.setLeftSpeed(motorControl.leftSpeed);
                motors.setRightSpeed(motorControl.rightSpeed);
                if (motorControl.duration)
                    motors.setDuration(motorControl.duration * 1000);
            }
            else
            {
                motors.moveDistCm(motorControl.driveSpeed, motorControl.distance,
                                  motorControl.driveDir);
            }
        }
        motorControl.update = false;
    }

    if (curtime > statusSendDelay)
    {
        statusSendDelay = curtime + 500;

        TWIStartMessage(MSG_SHARPIR);
        for (uint8_t i=0; i<SHARPIR_END; ++i)
            Wire.write(sharpIR[i].getDistance());
        Wire.endTransmission();

        TWIStartMessage(MSG_MOTOR_TARGETPOWER);
        for (uint8_t i=0; i<MOTOR_END; ++i)
            Wire.write(motors.getTargetPower(static_cast<EMotor>(i)));
        Wire.endTransmission();

        TWIStartMessage(MSG_MOTOR_SETPOWER);
        for (uint8_t i=0; i<MOTOR_END; ++i)
            Wire.write(motors.getSetPower(static_cast<EMotor>(i)));
        Wire.endTransmission();

        TWIStartMessage(MSG_MOTOR_TARGETSPEED);
        for (uint8_t i=0; i<MOTOR_END; ++i)
            TWISendInt(motors.getTargetSpeed(static_cast<EMotor>(i)));
        Wire.endTransmission();

        TWIStartMessage(MSG_MOTOR_TARGETDIST);
        for (uint8_t i=0; i<MOTOR_END; ++i)
            TWISendLong(motors.getTargetDistance(static_cast<EMotor>(i)));
        Wire.endTransmission();

        TWIStartMessage(MSG_MOTOR_CURRENT);
        for (uint8_t i=0; i<MOTOR_END; ++i)
            TWISendInt(motors.getCurrent(static_cast<EMotor>(i)));
        Wire.endTransmission();

        TWIStartMessage(MSG_ENCODER_SPEED);
        for (uint8_t i=0; i<ENC_END; ++i)
            TWISendInt(encoders.getSpeed(static_cast<EEncoder>(i)));
        Wire.endTransmission();

        TWIStartMessage(MSG_ENCODER_DISTANCE);
        for (uint8_t i=0; i<ENC_END; ++i)
            TWISendLong(encoders.getDist(static_cast<EEncoder>(i)));
        Wire.endTransmission();

        TWIStartMessage(MSG_SERVO);
        Wire.write(getServo().read());
        Wire.endTransmission();

        TWIStartMessage(MSG_BATTERY);
        TWISendInt(analogRead(PIN_BATTERY)); // UNDONE
        Wire.endTransmission();

        // UNDONE
        int16_t pitch, roll;
        uint16_t heading;
        getPitchRollHeading(pitch, roll, heading);
        TWIStartMessage(MSG_IMU);
        TWISendInt(pitch);
        TWISendInt(roll);
        TWISendInt(heading);
        Wire.endTransmission();
    }
}
