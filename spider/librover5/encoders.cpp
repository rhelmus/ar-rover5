#include "constants.h"
#include "encoders.h"
#include "librover5.h"

#include <Arduino.h>
#include <LSM303.h>

CEncoders encoders;

namespace {

// Encoder code based on LMR: http://letsmakerobots.com/node/24031

const int8_t QEM[16] = { 0, -1, 1, /*2*/0, 1, 0, /*2*/0, -1, -1, /*2*/0, 0, 1, /*2*/0, 1, -1, 0};
volatile uint8_t prevEncReading[ENC_END];
volatile int16_t encTicks[ENC_END];
volatile uint32_t encAbsDist[ENC_END];
volatile int32_t encDist[ENC_END];

volatile bool ledon;

void intEncLB(void)
{
    const uint8_t reading = ((PINC & (1<<PC4)) != 0) * 2 +
            ((PINC & (1<<PC3)) != 0);
    const int8_t v = QEM[prevEncReading[ENC_LB] * 4 + reading];

    encTicks[ENC_LB] -= v; // Note: inverted
    encAbsDist[ENC_LB] += abs(v);
    encDist[ENC_LB] += v;

    prevEncReading[ENC_LB] = reading;
}

void intEncLF(void)
{
    const uint8_t reading = ((PINC & (1<<PC6)) != 0) * 2 +
            ((PINC & (1<<PC5)) != 0);
    const int8_t v = QEM[prevEncReading[ENC_LF] * 4 + reading];

    encTicks[ENC_LF] += v;
    encAbsDist[ENC_LF] += abs(v);
    encDist[ENC_LF] += v;

    prevEncReading[ENC_LF] = reading;
}

void intEncRB(void)
{
    const uint8_t reading = ((PINC & (1<<PC0)) != 0) * 2 +
            ((PIND & (1<<PD7)) != 0);
    const int8_t v = QEM[prevEncReading[ENC_RB] * 4 + reading];

    encTicks[ENC_RB] += v;
    encAbsDist[ENC_RB] += abs(v);
    encDist[ENC_RB] += v;

    prevEncReading[ENC_RB] = reading;
}

void intEncRF(void)
{
    const uint8_t reading = ((PINC & (1<<PC2)) != 0) * 2 +
            ((PINC & (1<<PC1)) != 0);
    const int8_t v = QEM[prevEncReading[ENC_RF] * 4 + reading];

    encTicks[ENC_RF] -= v; // Note: inverted
    encAbsDist[ENC_RF] += abs(v);
    encDist[ENC_RF] += v;

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

    // Initialize theta from compass
    getCompass().read();
    theta = 0.0;//(getCompass().heading() * PI / 180.0);
}

void CEncoders::update()
{
    // Update position
    const int32_t le = ((::encTicks[ENC_LB] + ::encTicks[ENC_LF]) / 2);
    const int32_t re = ((::encTicks[ENC_RB] + ::encTicks[ENC_RF]) / 2);

    const float ldist = le / ENC_PULSES_CM, rdist = re / ENC_PULSES_CM;

    theta += ((ldist - rdist) / 2.0 / WHEEL_BASE);

    // Clamp
    const float PI2 = PI * 2.0;
    while (true)
    {
        if (theta > PI2)
            theta -= PI2;
        else if (theta < 0)
            theta += PI2;
        else
            break;
    }

    const float dist = (ldist + rdist) / 2.0;
    xPos += dist * sin(theta);
    yPos += dist * cos(theta);

    // Update speed & reset ticks
    for (uint8_t e=0; e<ENC_END; ++e)
    {
        encSpeed[e] = ::encTicks[e];
        ::encTicks[e] = 0;
    }
}

bool CEncoders::isMoving() const
{
    for (uint8_t e=0; e<ENC_END; ++e)
    {
        if (encSpeed[e])
            return true;
    }

    return false;
}

uint32_t CEncoders::getAbsDist(EEncoder e) const
{
    return encAbsDist[e];
}

int32_t CEncoders::getDist(EEncoder e) const
{
    // Left-back and right-front encoders are inverted
    if ((e == ENC_LB) || (e == ENC_RF))
        return -encDist[e];

    return encDist[e];
}

void CEncoders::resetDist()
{
    memset((void *)encAbsDist, 0, sizeof(encAbsDist));
//    memset((void *)encDist, 0, sizeof(encDist));
}
