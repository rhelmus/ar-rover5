#include "constants.h"
#include "sharpir.h"

#include <Arduino.h>

CSharpIR sharpIR[SHARPIR_END] =
{
    CSharpIR(PIN_SHARP_IR_TURRET, CSharpIR::GP2Y0A02YK),
    CSharpIR(PIN_SHARP_IR_LEFT, CSharpIR::GP2Y0A21YK),
    CSharpIR(PIN_SHARP_IR_LEFT_FW, CSharpIR::GP2Y0A21YK),
    CSharpIR(PIN_SHARP_IR_FW, CSharpIR::GP2Y0A21YK),
    CSharpIR(PIN_SHARP_IR_RIGHT, CSharpIR::GP2Y0A21YK),
    CSharpIR(PIN_SHARP_IR_RIGHT_FW, CSharpIR::GP2Y0A21YK)
};


uint8_t CSharpIR::getDistReading() const
{
    const uint16_t adc = analogRead(ADCPin);

    // R = (1 / (m * V + b)) - k, see http://www.acroname.com/robotics/info/articles/irlinear/irlinear.html

    if (model == GP2Y0A21YK)
        return (1 / (GP2Y0A21YK_A * adc + GP2Y0A21YK_B)) - GP2Y0A21YK_K;

    return (1 / (GP2Y0A02YK_A * adc + GP2Y0A02YK_B)) - GP2Y0A02YK_K;
}

bool CSharpIR::readingIsHit(uint8_t r) const
{
    if (model == GP2Y0A21YK)
        return ((r >= 10) && (r <= 80));

    return ((r >= 20) && (r <= 150));
}

void CSharpIR::clearReadings()
{
    memset(readings, 0, sizeof(readings));
    readingsIndex = readingCount = hitCount = hitDistTotal = 0;
}

void CSharpIR::update()
{
    // rolling data storage based on http://arduino.cc/en/Tutorial/Smoothing

    const uint32_t curtime = millis();

    if (curtime > updateDelay)
    {
        updateDelay = curtime + 50;

        if (readingIsHit(readings[readingsIndex]))
        {
            --hitCount;
            hitDistTotal -= readings[readingsIndex];
        }

        readings[readingsIndex] = getDistReading();

        if (readingIsHit(readings[readingsIndex]))
        {
            ++hitCount;
            hitDistTotal += readings[readingsIndex];
        }

        ++readingsIndex;
        if (readingsIndex >= READINGS_SIZE)
            readingsIndex = 0;

        ++readingCount;
    }
}


void resetSharpIRSensors()
{
    for (uint8_t i=0; i<SHARPIR_END; ++i)
        sharpIR[i].clearReadings();
}
