#include <Wire.h>
#include <LSM303.h>
#include <Servo.h>
#include <TimerOne.h>

#include <librover5.h>

namespace {

struct SState
{
    uint32_t checkIRDelay;
    uint8_t IRReadings, IRHits;
    int8_t servoStep;
} state;

void resetState(void)
{
    state.checkIRDelay = state.IRReadings = state.IRHits = 0;
    state.servoStep = 1;
}

// UNDONE: Put somewhere else
inline uint8_t checkSum(uint8_t b) { return b * 2; }

}

void setup()
{
    Serial.begin(115200);
    Serial2.begin(1200);

    initRover5();

//    motors.enable();

    resetState();

    Serial.println("Initialized");
}

void loop()
{
    const uint32_t curtime = millis();

    if (state.checkIRDelay < curtime)
    {
        state.checkIRDelay = curtime + 50;

        if (Serial2.available() >= 2)
        {
            const uint8_t msg = Serial2.read();

            Serial.print("Got SIR!(nc): "); Serial.write(msg); Serial.write('\n');

            if (Serial2.peek() == checkSum(msg)) // Check validity
            {
                Serial2.read(); // Pop
                ++state.IRHits;
                Serial.print("Got SIR!: "); Serial.write(msg); Serial.write('\n');
            }
        }
        else
            digitalWrite(13, LOW);

#if 0
        if (Serial2.available())
        {
            digitalWrite(13, HIGH);
            ++state.IRHits;
            while (Serial2.available())
                Serial2.read();
        }
        else
            digitalWrite(13, LOW);
#endif
        ++state.IRReadings;

        if (state.IRReadings > 6)
        {
            Servo &servo = getLowerServo();
            int16_t newspos = servo.read() + state.servoStep;

            if (newspos < 0)
            {
                newspos = 0;
                state.servoStep = 1;
            }
            else if (newspos >= 180)
            {
                newspos = 179;
                state.servoStep = -1;
            }

            servo.write(newspos);

            if (state.IRHits >= 4)
            {
                Serial.print("spos: "); Serial.println(servo.read(), DEC);
            }

            state.IRReadings = state.IRHits = 0;
        }
    }

    rover5Task();
}
