#include "constants.h"
#include "encoders.h"

#include <Arduino.h>

CEncoders encoders;

namespace {

// Encoder code based on LMR: http://letsmakerobots.com/node/24031

const int8_t QEM[16] = { 0, -1, 1, /*2*/0, 1, 0, /*2*/0, -1, -1, /*2*/0, 0, 1, /*2*/0, 1, -1, 0};
volatile uint8_t prevEncReading[ENC_COUNT];
volatile int16_t encTicks[ENC_COUNT];

void intEncLB(void)
{
//    const int8_t reading = digitalRead(PIN_ENC_LB_SIGN1) * 2 +
//            digitalRead(PIN_ENC_LB_SIGN2);

    const uint8_t reading = ((PINC & (1<<PC4)) != 0) * 2 +
            ((PINC & (1<<PC3)) != 0);

    encTicks[ENC_LB] += QEM[prevEncReading[ENC_LB] * 4 + reading];
    prevEncReading[ENC_LB] = reading;
}

void intEncLF(void)
{
//    const int8_t reading = digitalRead(PIN_ENC_LF_SIGN1) * 2 +
//            digitalRead(PIN_ENC_LF_SIGN2);

    const uint8_t reading = ((PINC & (1<<PC6)) != 0) * 2 +
            ((PINC & (1<<PC5)) != 0);

    encTicks[ENC_LF] += QEM[prevEncReading[ENC_LF] * 4 + reading];
    prevEncReading[ENC_LF] = reading;
}

void intEncRB(void)
{
//    const int8_t reading = digitalRead(PIN_ENC_RB_SIGN1) * 2 +
//            digitalRead(PIN_ENC_RB_SIGN2);

    const uint8_t reading = ((PINC & (1<<PC0)) != 0) * 2 +
            ((PIND & (1<<PD7)) != 0);

    encTicks[ENC_RB] += QEM[prevEncReading[ENC_RB] * 4 + reading];
    prevEncReading[ENC_RB] = reading;
}

void intEncRF(void)
{
//    const int8_t reading = digitalRead(PIN_ENC_RF_SIGN1) * 2 +
//            digitalRead(PIN_ENC_RF_SIGN2);

    const uint8_t reading = ((PINC & (1<<PC2)) != 0) * 2 +
            ((PINC & (1<<PC1)) != 0);

    encTicks[ENC_RF] += QEM[prevEncReading[ENC_RF] * 4 + reading];
    prevEncReading[ENC_RF] = reading;
}

}

void CEncoders::init()
{
    pinMode(PIN_ENC_LB_SIGN1, INPUT);
    pinMode(PIN_ENC_LB_SIGN2, INPUT);
    pinMode(PIN_ENC_LF_SIGN1, INPUT);
    pinMode(PIN_ENC_LF_SIGN2, INPUT);
    pinMode(PIN_ENC_RB_SIGN1, INPUT);
    pinMode(PIN_ENC_RB_SIGN2, INPUT);
    pinMode(PIN_ENC_RF_SIGN1, INPUT);
    pinMode(PIN_ENC_RF_SIGN2, INPUT);

    attachInterrupt(INT_ENC_LB, intEncLB, CHANGE);
    attachInterrupt(INT_ENC_LF, intEncLF, CHANGE);
    attachInterrupt(INT_ENC_RB, intEncRB, CHANGE);
    attachInterrupt(INT_ENC_RF, intEncRF, CHANGE);
}

void CEncoders::update()
{
    for (uint8_t e=0; e<ENC_COUNT; ++e)
    {
        // Left-front and right-back encoders are inverted
        if ((e == ENC_LF) || (e == ENC_RB))
            encSpeed[e] = -(::encTicks[e]);
        else
            encSpeed[e] = ::encTicks[e];

        ::encTicks[e] = 0;

        /*Serial.print("enc "); Serial.print(e, DEC);
        Serial.print(": "); Serial.println(encSpeed[e], DEC);*/
    }
}
