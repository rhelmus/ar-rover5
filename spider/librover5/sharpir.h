#ifndef SHARPIR_H
#define SHARPIR_H

#include "../../shared/shared.h"

#include <stdint.h>

class CSharpIR
{
public:
    enum EModel { GP2Y0A21YK, GP2Y0A02YK };

private:
    enum { READINGS_SIZE = 6 };

    uint8_t ADCPin;
    EModel model;
    uint8_t readings[READINGS_SIZE];
    uint16_t readingsTotal, readingsCount;
    uint8_t readingsIndex;
    uint32_t updateDelay;

    uint8_t getDistReading(void) const;

public:
    CSharpIR(uint8_t p, EModel m)
        : ADCPin(p), model(m), readingsTotal(0), readingsCount(0), readingsIndex(0),
          updateDelay(0) { clearReadings(); }

    void clearReadings(void);
    uint8_t getAvgDist(void) const { return readingsTotal / min(readingsCount, READINGS_SIZE); }
    uint8_t getMedianDist(void) const;
    void update(void);
};

extern CSharpIR sharpIR[SHARPIR_END];

#endif // SHARPIR_H
