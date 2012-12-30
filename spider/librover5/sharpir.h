#ifndef SHARPIR_H
#define SHARPIR_H

#include "../../shared/shared.h"

#include <stdint.h>

class CSharpIR
{
public:
    enum EModel { GP2Y0A21YK, GP2Y0A02YK };

private:
    uint8_t ADCPin;
    EModel model;

public:
    CSharpIR(uint8_t p, EModel m) : ADCPin(p), model(m) { }

    uint8_t getDistance(void) const;
};

extern CSharpIR sharpIR[SHARPIR_END];

#endif // SHARPIR_H
