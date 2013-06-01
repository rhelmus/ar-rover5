#ifndef SHARPIR_H
#define SHARPIR_H

#include "../../shared/shared.h"
#include "utils.h"

#include <stdint.h>

#include <Arduino.h>

class CSharpIR
{
public:
    enum EModel { GP2Y0A21YK, GP2Y0A02YK };

private:
    enum { READINGS_SIZE = 6 };

    uint8_t ADCPin;
    EModel model;
    CRollingAverage<READINGS_SIZE, uint8_t> readings;
    uint16_t totReadingCount;
    uint32_t updateDelay;

    uint8_t getDistReading(void) const;
    bool readingIsHit(uint8_t r) const;

public:
    CSharpIR(uint8_t p, EModel m)
        : ADCPin(p), model(m), totReadingCount(0), updateDelay(0) { clearReadings(); }

    void clearReadings(void);
    uint8_t getAvgDist(void) const { return readings.average(); }
    uint8_t getTotReadingCount(void) const { return totReadingCount; }
    uint8_t getHitCount(void) const { return readings.getCount(); }
    void update(void);
};

extern CSharpIR sharpIR[SHARPIR_END];

void resetSharpIRSensors(void);

#endif // SHARPIR_H
