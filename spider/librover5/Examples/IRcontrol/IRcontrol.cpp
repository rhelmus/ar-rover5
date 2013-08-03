#include <Wire.h>
#include <LSM303.h>
#include <Servo.h>
#include <TimerOne.h>

#include <librover5.h>

#include "../../../../shared/IRcontrol.h"

void setup()
{
    Serial.begin(115200);
    Serial2.begin(1200);

    initRover5();

    motors.enable();

    Serial.println("Initialized");
}

void loop()
{
    static uint32_t checkdelay, updatedelay;
    static SIRMessage currentmsg;
    static bool gotmessage, initleds = true;
    const uint32_t curtime = millis();

    if (checkdelay < curtime)
    {
        checkdelay = curtime + 5;

#if 1
        if (Serial2.available() >= 2)
        {
            SIRMessage msg(Serial2.read());

            /*Serial.print("Got SIR data: "); Serial.print(msg.byte, DEC);
            Serial.print(", "); Serial.println(Serial2.peek(), DEC);*/

            if (Serial2.peek() == IRCheckSum(msg)) // Check validity
            {
                Serial2.read(); // Pop

                Serial.println("Got SIR!: "); Serial.println(msg.command, DEC);

                if (!gotmessage || (msg.byte != currentmsg.byte))
                {
                    gotmessage = true;
                    updatedelay = curtime + 200;
                    currentmsg.byte = msg.byte;
                }
            }
        }
#else
        if (Serial2.available())
        {
            Serial.print("SIR:");
            while (Serial2.available())
                Serial.write(Serial2.read());
            Serial.println("");
        }
#endif


    }

    if ((updatedelay != 0) && (updatedelay < curtime))
    {
        updatedelay = 0;

        const uint8_t mspeed = map(currentmsg.speed, 0, 15, MIN_MOTOR_POWER,
                                   MAX_MOTOR_POWER);

        if (currentmsg.command == IRCMD_FWD)
            motors.move(mspeed, MDIR_FWD);
        else if (currentmsg.command == IRCMD_BWD)
            motors.move(mspeed, MDIR_BWD);
#ifdef MECANUM_MOVEMENT
        else if (currentmsg.translate)
        {
            if (currentmsg.command == IRCMD_LEFT)
                motors.translate(mspeed, TRDIR_LEFT);
            else if (currentmsg.command == IRCMD_LEFT_FWD)
                motors.translate(mspeed, TRDIR_LEFT_FWD);
            else if (currentmsg.command == IRCMD_LEFT_BWD)
                motors.translate(mspeed, TRDIR_LEFT_BWD);
            else if (currentmsg.command == IRCMD_RIGHT)
                motors.translate(mspeed, TRDIR_RIGHT);
            else if (currentmsg.command == IRCMD_RIGHT_FWD)
                motors.translate(mspeed, TRDIR_RIGHT_FWD);
            else if (currentmsg.command == IRCMD_RIGHT_BWD)
                motors.translate(mspeed, TRDIR_RIGHT_BWD);
        }
#endif
        else
        {
            if (currentmsg.command == IRCMD_LEFT)
                motors.turn(mspeed, TDIR_LEFT);
            else if (currentmsg.command == IRCMD_LEFT_FWD)
            {
                motors.setLeftSpeed(0);
                motors.setRightSpeed(mspeed);
                motors.setRightDirection(MDIR_FWD);
            }
            else if (currentmsg.command == IRCMD_LEFT_BWD)
            {
                motors.setLeftSpeed(0);
                motors.setRightSpeed(mspeed);
                motors.setRightDirection(MDIR_BWD);
            }
            else if (currentmsg.command == IRCMD_RIGHT)
                motors.turn(mspeed, TDIR_RIGHT);
            else if (currentmsg.command == IRCMD_RIGHT_FWD)
            {
                motors.setLeftSpeed(mspeed);
                motors.setLeftDirection(MDIR_FWD);
                motors.setRightSpeed(0);
            }
            else if (currentmsg.command == IRCMD_RIGHT_BWD)
            {
                motors.setLeftSpeed(mspeed);
                motors.setLeftDirection(MDIR_BWD);
                motors.setRightSpeed(0);
            }
        }

        if (initleds)
        {
            digitalWrite(PIN_FRONTLEDS, 1);
            initleds = false;
        }

        motors.setDuration(500);
        gotmessage = false;
    }

    rover5Task();
}
