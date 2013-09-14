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

#define REM_SERIAL Serial2

namespace {

struct SMotorControl
{
    bool update;
    enum { MOVE_STRAIGHT, MOVE_TURN, MOVE_TRANSLATE, MOVE_NONE } moveType;

    union
    {
        uint8_t leftSpeed;
        uint8_t driveSpeed;
        uint8_t turnSpeed;
#ifdef MECANUM_MOVEMENT
        uint8_t translateSpeed;
#endif
    };
    uint8_t rightSpeed;

    union
    {
        EMotorDirection mdirLeft;
        EMotorDirection driveDir;
        ETurnDirection turnDir;
#ifdef MECANUM_MOVEMENT
        ETranslateDirection translateDir;
#endif
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

template <uint8_t size> class CSerialSendHelper
{
    uint8_t buffer[size];

    uint8_t getLastIndex(void) const { return buffer[1] + 1; }

public:
    void start(EMessage m)
    {
        // Format: <msg start marker>, <msg size>, <msg type and other data>, <msg end marker>
        buffer[0] = MSG_STARTMARKER;
        buffer[2] = static_cast<uint8_t>(m);
        buffer[1] = 1; // Message size so far
    }

    void end(void)
    {
        buffer[getLastIndex() + 1] = MSG_ENDMARKER;
        REM_SERIAL.write(buffer, getLastIndex() + 2);
    }

    void pushByte(uint8_t b)
    {
        ++buffer[1];
        buffer[getLastIndex()] = b;
    }

    void pushBytes(uint8_t *bytes, uint8_t s)
    {
        for (uint8_t i=0; i<s; ++i)
            pushByte(bytes[i]);
    }

    void pushInt(uint16_t i)
    {
        uint8_t buf[2];
        intToBytes(i, buf);
        pushBytes(buf, 2);
    }

    void pushLong(uint32_t l)
    {
        uint8_t buf[4];
        longToBytes(l, buf);
        pushBytes(buf, 4);
    }

    void pushFloat(float f)
    {
        pushLong(static_cast<int32_t>(f * 1000.0)); // 3 decimal accuracy
    }
};

void getPitchRollHeading(int16_t &p, int16_t &r, uint16_t &h)
{
    // From LSM303 application note
    // NOTE: pitch&roll are switched
    LSM303::vector vec = getCompass().a;
    getCompass().vector_normalize(&vec);
    const float fp = asin(-vec.x);
    r = round(fp * 180.0 / M_PI);
    p = round(asin(-vec.y / cos(fp)) * 180.0 / M_PI);

    if (getCompass().m.x < getCompass().m_min.x)
        getCompass().m_min.x = getCompass().m.x;
    if (getCompass().m.y < getCompass().m_min.y)
        getCompass().m_min.y = getCompass().m.y;
    if (getCompass().m.z < getCompass().m_min.z)
        getCompass().m_min.z = getCompass().m.z;
    if (getCompass().m.x > getCompass().m_max.x)
        getCompass().m_max.x = getCompass().m.x;
    if (getCompass().m.y > getCompass().m_max.y)
        getCompass().m_max.y = getCompass().m.y;
    if (getCompass().m.z > getCompass().m_max.z)
        getCompass().m_max.z = getCompass().m.z;

    h = getCompass().heading();
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
        motorControl.moveType = SMotorControl::MOVE_STRAIGHT;
    }
    else if (msg == MSG_CMD_DRIVEDIST)
    {
        motorControl.update = true;
        motorControl.driveSpeed = Wire.read();
        motorControl.distance = bytesToInt(Wire.read(), Wire.read());
        motorControl.driveDir = static_cast<EMotorDirection>(Wire.read());
        motorControl.duration = 0;
        motorControl.moveType = SMotorControl::MOVE_STRAIGHT;
    }
    else if (msg == MSG_CMD_TURN)
    {
        motorControl.update = true;
        motorControl.turnSpeed = Wire.read();
        motorControl.turnDir = static_cast<ETurnDirection>(Wire.read());
        motorControl.duration = bytesToInt(Wire.read(), Wire.read());
        motorControl.moveType = SMotorControl::MOVE_TURN;
        motorControl.angle = 0;
    }
    else if (msg == MSG_CMD_TURNANGLE)
    {
        motorControl.update = true;
        motorControl.turnSpeed = Wire.read();
        motorControl.angle = bytesToInt(Wire.read(), Wire.read());
        motorControl.turnDir = static_cast<ETurnDirection>(Wire.read());
        motorControl.duration = 0;
        motorControl.moveType = SMotorControl::MOVE_TURN;
    }
#ifdef MECANUM_MOVEMENT
    else if (msg == MSG_CMD_TRANSLATE)
    {
        motorControl.update = true;
        motorControl.translateSpeed = Wire.read();
        motorControl.translateDir = static_cast<ETranslateDirection>(Wire.read());
        motorControl.duration = bytesToInt(Wire.read(), Wire.read());
        motorControl.moveType = SMotorControl::MOVE_TRANSLATE;
        motorControl.distance = 0;
    }
    else if (msg == MSG_CMD_TRANSLATEDIST)
    {
        motorControl.update = true;
        motorControl.translateSpeed = Wire.read();
        motorControl.distance = bytesToInt(Wire.read(), Wire.read());
        motorControl.translateDir = static_cast<ETranslateDirection>(Wire.read());
        motorControl.duration = 0;
        motorControl.moveType = SMotorControl::MOVE_TRANSLATE;
    }
#endif
    else if (msg == MSG_CMD_STOP)
    {
        motorControl.update = true;
        motorControl.moveType = SMotorControl::MOVE_NONE;
    }
    else if (msg == MSG_CMD_FRONTLEDS)
    {
        digitalWrite(PIN_FRONTLEDS, Wire.read());
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


void CRemoteInterface::updateMotorControl()
{
    if (motorControl.moveType == SMotorControl::MOVE_STRAIGHT)
    {
        if (!motorControl.distance)
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
    else if (motorControl.moveType == SMotorControl::MOVE_TURN)
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
#ifdef MECANUM_MOVEMENT
    else if (motorControl.moveType == SMotorControl::MOVE_TRANSLATE)
    {
        if (!motorControl.distance)
        {
            motors.translate(motorControl.translateSpeed, motorControl.translateDir);
            if (motorControl.duration)
                motors.setDuration(motorControl.duration * 1000);
        }
        else
            motors.translateCm(motorControl.translateSpeed, motorControl.distance,
                               motorControl.translateDir);
    }
#endif
    else // MOVE_NONE
        motors.stop();
}

void CRemoteInterface::sendStatus()
{
    CSerialSendHelper<24> sender; // NOTE: increase if larger messages are desired

    // Don't send everything at once, divide it in blocks to prevent blocking the processor
    sender.start(MSG_SHARPIR);
    for (uint8_t i=0; i<SHARPIR_END; ++i)
        sender.pushByte(sharpIR[i].getAvgDist());
    sender.end();

    sender.start(MSG_MOTOR_TARGETPOWER);
    for (uint8_t i=0; i<MOTOR_END; ++i)
        sender.pushByte(motors.getTargetPower(static_cast<EMotor>(i)));
    sender.end();

    sender.start(MSG_MOTOR_SETPOWER);
    for (uint8_t i=0; i<MOTOR_END; ++i)
        sender.pushByte(motors.getSetPower(static_cast<EMotor>(i)));
    sender.end();

    sender.start(MSG_MOTOR_TARGETSPEED);
    for (uint8_t i=0; i<MOTOR_END; ++i)
        sender.pushInt(motors.getTargetSpeed(static_cast<EMotor>(i)));
    sender.end();

    sender.start(MSG_MOTOR_TARGETDIST);
    for (uint8_t i=0; i<MOTOR_END; ++i)
        sender.pushLong(motors.getTargetDistance(static_cast<EMotor>(i)));
    sender.end();

    sender.start(MSG_MOTOR_CURRENT);
    for (uint8_t i=0; i<MOTOR_END; ++i)
        sender.pushInt(motors.getCurrent(static_cast<EMotor>(i)));
    sender.end();

    sender.start(MSG_ENCODER_SPEED);
    for (uint8_t i=0; i<ENC_END; ++i)
        sender.pushInt(encoders.getSpeed(static_cast<EEncoder>(i)));
    sender.end();

    sender.start(MSG_ENCODER_DISTANCE);
    for (uint8_t i=0; i<ENC_END; ++i)
        sender.pushLong(encoders.getAbsDist(static_cast<EEncoder>(i)));
    sender.end();

    sender.start(MSG_ODOMETRY);
    sender.pushFloat(encoders.getXPos());
    sender.pushFloat(encoders.getYPos());
    sender.pushFloat(encoders.getRotation());
    sender.end();

    sender.start(MSG_SERVO);
    sender.pushByte(getLowerServo().read());
    sender.end();

    sender.start(MSG_BATTERY);
    sender.pushInt(analogRead(PIN_BATTERY)); // UNDONE
    sender.end();

    // UNDONE
    int16_t pitch, roll;
    uint16_t heading;
    getPitchRollHeading(pitch, roll, heading);
    sender.start(MSG_IMU);
    sender.pushInt(pitch);
    sender.pushInt(roll);
    sender.pushInt(heading);
    sender.end();
}

void CRemoteInterface::checkForCommands()
{
    while (REM_SERIAL.available())
    {
        const uint8_t b = REM_SERIAL.read();
        if (!receivedSerRecMsgStart)
            receivedSerRecMsgStart = (b == MSG_STARTMARKER);
        else if (!receivedSerRecMsgSize)
        {
            receivedSerRecMsgSize = true;
            serRecMsgSize = b;
        }
        else if (serRecMsgBytesRead < serRecMsgSize)
        {
            if (currentSerRecMsg == MSG_NONE)
                currentSerRecMsg = static_cast<EMessage>(b);
            else
                tempSerialMsgBuffer.push(b);
            ++serRecMsgBytesRead;
        }
        else
        {
            // Msg got through OK?
            if (b == MSG_ENDMARKER)
            {
                // UNDONE
#if 0
                // First check special cases that are not passed through
                if (currentSerRecMsg == MSG_PONG)
                {
                    tempSerialMsgBuffer.clear();
//                            wdt_reset();
                }
                else if (currentSerRecMsg == MSG_CNTRL_DISCONNECT)
                {
                    tempSerialMsgBuffer.clear();
                    Serial.println("cntrl disconnect");
                    btConnected = false;
                    // Wait for a bit for actual disconnection before
                    // updating status
                    ignoreconupdatedelay = curtime + 3000;
                }
                else
#endif
                {
                    handleCommand(currentSerRecMsg);
                }
            }

            receivedSerRecMsgStart = receivedSerRecMsgSize = false;
            serRecMsgSize = 0;
            serRecMsgBytesRead = 0;
            currentSerRecMsg = MSG_NONE;
            tempSerialMsgBuffer.clear();
        }
    }
}

void CRemoteInterface::handleCommand(EMessage cmd)
{
    if (cmd == MSG_CMD_MOTORSPEED)
    {
        motorControl.update = true;
        motorControl.leftSpeed = tempSerialMsgBuffer.pop();
        motorControl.rightSpeed = tempSerialMsgBuffer.pop();
        motorControl.mdirLeft = static_cast<EMotorDirection>(tempSerialMsgBuffer.pop());
        motorControl.mdirRight = static_cast<EMotorDirection>(tempSerialMsgBuffer.pop());
        motorControl.duration = bytesToInt(tempSerialMsgBuffer.pop(), tempSerialMsgBuffer.pop());
        motorControl.distance = 0;
        motorControl.moveType = SMotorControl::MOVE_STRAIGHT;
    }
    else if (cmd == MSG_CMD_DRIVEDIST)
    {
        motorControl.update = true;
        motorControl.driveSpeed = tempSerialMsgBuffer.pop();
        motorControl.distance = bytesToInt(tempSerialMsgBuffer.pop(), tempSerialMsgBuffer.pop());
        motorControl.driveDir = static_cast<EMotorDirection>(tempSerialMsgBuffer.pop());
        motorControl.duration = 0;
        motorControl.moveType = SMotorControl::MOVE_STRAIGHT;
    }
    else if (cmd == MSG_CMD_TURN)
    {
        motorControl.update = true;
        motorControl.turnSpeed = tempSerialMsgBuffer.pop();
        motorControl.turnDir = static_cast<ETurnDirection>(tempSerialMsgBuffer.pop());
        motorControl.duration = bytesToInt(tempSerialMsgBuffer.pop(), tempSerialMsgBuffer.pop());
        motorControl.moveType = SMotorControl::MOVE_TURN;
        motorControl.angle = 0;
    }
    else if (cmd == MSG_CMD_TURNANGLE)
    {
        motorControl.update = true;
        motorControl.turnSpeed = tempSerialMsgBuffer.pop();
        motorControl.angle = bytesToInt(tempSerialMsgBuffer.pop(), tempSerialMsgBuffer.pop());
        motorControl.turnDir = static_cast<ETurnDirection>(tempSerialMsgBuffer.pop());
        motorControl.duration = 0;
        motorControl.moveType = SMotorControl::MOVE_TURN;
    }
#ifdef MECANUM_MOVEMENT
    else if (cmd == MSG_CMD_TRANSLATE)
    {
        motorControl.update = true;
        motorControl.translateSpeed = tempSerialMsgBuffer.pop();
        motorControl.translateDir = static_cast<ETranslateDirection>(tempSerialMsgBuffer.pop());
        motorControl.duration = bytesToInt(tempSerialMsgBuffer.pop(), tempSerialMsgBuffer.pop());
        motorControl.moveType = SMotorControl::MOVE_TRANSLATE;
        motorControl.distance = 0;
    }
    else if (cmd == MSG_CMD_TRANSLATEDIST)
    {
        motorControl.update = true;
        motorControl.translateSpeed = tempSerialMsgBuffer.pop();
        motorControl.distance = bytesToInt(tempSerialMsgBuffer.pop(), tempSerialMsgBuffer.pop());
        motorControl.translateDir = static_cast<ETranslateDirection>(tempSerialMsgBuffer.pop());
        motorControl.duration = 0;
        motorControl.moveType = SMotorControl::MOVE_TRANSLATE;
    }
#endif
    else if (cmd == MSG_CMD_STOP)
    {
        motorControl.update = true;
        motorControl.moveType = SMotorControl::MOVE_NONE;
    }
    else if (cmd == MSG_CMD_FRONTLEDS)
    {
        digitalWrite(PIN_FRONTLEDS, tempSerialMsgBuffer.pop());
    }
}

void CRemoteInterface::init()
{
    Serial2.begin(115200);
}

void CRemoteInterface::update()
{
    if (motorControl.update)
    {
        updateMotorControl();
        motorControl.update = false;
    }

    const uint32_t curtime = millis();
    if (curtime > statusSendDelay)
    {
        statusSendDelay = curtime + 500;
        sendStatus();
    }

    if (REM_SERIAL.available())
        checkForCommands();
}
