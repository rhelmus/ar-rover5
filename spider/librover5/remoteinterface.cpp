#include "../../shared/shared.h"
#include "encoders.h"
#include "librover5.h"
#include "motors.h"
#include "remoteinterface.h"
#include "sharpir.h"

#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>

CRemoteInterface remoteInterface;

namespace {

#if 0
uint32_t TWIReqLong(EMessage m)
{
    Wire.beginTransmission(BRIDGE_TWI_ADDRESS);
    Wire.write(m);
    Wire.endTransmission();

    Wire.requestFrom(BRIDGE_TWI_ADDRESS, 4);
    return bytesToLong(Wire.read(), Wire.read(),
                       Wire.read(), Wire.read());
}

void TWISendBytes(EMessage m, uint8_t *buf, uint8_t n)
{
    Wire.beginTransmission(BRIDGE_TWI_ADDRESS);
    Wire.write(m);
    Wire.write(buf, n);
    Wire.endTransmission();
}

void TWISendByte(EMessage m, uint8_t b)
{
    TWISendBytes(m, &b, 1);
}

void TWISendInt(EMessage m, uint16_t i)
{
    uint8_t buf[2];
    intToBytes(i, buf);
    TWISendBytes(m, buf, 2);
}

void TWISendLong(EMessage m, uint32_t i)
{
    uint8_t buf[4];
    intToBytes(i, buf);
    TWISendBytes(m, buf, 4);
}

class CTWISender
{
    bool sended;

public:
    CTWISender(EMessage m) : sended(false)
    { Wire.beginTransmission(BRIDGE_TWI_ADDRESS); Wire.send(m); }
    ~CTWISender(void) { if (!sended) end(); }

    void end(void) { Wire.endTransmission(); }

    CTWISender &operator <<(uint8_t data)
    {
        Wire.write(data);
        return *this;
    }

    CTWISender &operator <<(uint16_t data)
    {
        uint8_t buf[2];
        intToBytes(data, buf);
        Wire.write(buf, 2);
        return *this;
    }

    CTWISender &operator <<(uint32_t data)
    {
        uint8_t buf[4];
        longToBytes(data, buf);
        Wire.write(buf, 4);
        return *this;
    }

    void sendData(uint8_t *data, bytes)
    {
        Wire.write(data, bytes);
        return *this;
    }
};

#endif

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

uint32_t TWIReadLong(void)
{
    return bytesToLong(Wire.read(), Wire.read(),
                       Wire.read(), Wire.read());
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

}


void CRemoteInterface::update()
{
    const uint32_t curtime = millis();
    if (curtime > updatePingDelay)
    {
        updatePingDelay = curtime + 100;
        TWIRequest(MSG_PING, 4);
        pingTime = TWIReadLong();

//        Serial.print("Pong: "); Serial.println(pingTime);
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
        TWISendInt(analogRead(PIN_BATTERY));
        Wire.endTransmission();
    }
}
