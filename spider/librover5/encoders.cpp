#include "constants.h"
#include "encoders.h"

#include <Arduino.h>

CEncoders encoders;

namespace {

// Encoder code based on LMR: http://letsmakerobots.com/node/24031

const int8_t QEM[16] = { 0, -1, 1, /*2*/0, 1, 0, /*2*/0, -1, -1, /*2*/0, 0, 1, /*2*/0, 1, -1, 0};
volatile uint8_t prevEncReading[ENC_COUNT];
volatile int16_t encTicks[ENC_COUNT];
volatile uint32_t encDist[ENC_COUNT];

void intEncLB(void)
{
    const uint8_t reading = ((PINC & (1<<PC4)) != 0) * 2 +
            ((PINC & (1<<PC3)) != 0);
    const int8_t v = QEM[prevEncReading[ENC_LB] * 4 + reading];

    encTicks[ENC_LB] += v;
    encDist[ENC_LB] += abs(v);

    prevEncReading[ENC_LB] = reading;
}

void intEncLF(void)
{
    const uint8_t reading = ((PINC & (1<<PC6)) != 0) * 2 +
            ((PINC & (1<<PC5)) != 0);
    const int8_t v = QEM[prevEncReading[ENC_LF] * 4 + reading];

    encTicks[ENC_LF] += v;
    encDist[ENC_LF] += abs(v);

    prevEncReading[ENC_LF] = reading;
}

void intEncRB(void)
{
    const uint8_t reading = ((PINC & (1<<PC0)) != 0) * 2 +
            ((PIND & (1<<PD7)) != 0);
    const int8_t v = QEM[prevEncReading[ENC_RB] * 4 + reading];

    encTicks[ENC_RB] += v;
    encDist[ENC_RB] += abs(v);

    prevEncReading[ENC_RB] = reading;
}

void intEncRF(void)
{
    const uint8_t reading = ((PINC & (1<<PC2)) != 0) * 2 +
            ((PINC & (1<<PC1)) != 0);
    const int8_t v = QEM[prevEncReading[ENC_RF] * 4 + reading];

    encTicks[ENC_RF] += v;
    encDist[ENC_RF] += abs(v);

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
        // Left-back and right-front encoders are inverted
        if ((e == ENC_LB) || (e == ENC_RF))
            encSpeed[e] = -(::encTicks[e]);
        else
            encSpeed[e] = ::encTicks[e];

        ::encTicks[e] = 0;
    }
}

uint32_t CEncoders::getDist(EEncoder e) const
{
    return encDist[e];
}

void CEncoders::resetDist()
{
    memset((void *)encDist, 0, sizeof(encDist));
}
