#ifndef SHARPIR_H
#define SHARPIR_H

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

enum
{
    SHARPIR_TURRET = 0,
    SHARPIR_LEFT,
    SHARPIR_LEFT_FW,
    SHARPIR_FW,
    SHARPIR_RIGHT,
    SHARPIR_RIGHT_FW,
    SHARPIR_END
};

extern CSharpIR sharpIR[SHARPIR_END];

#endif // SHARPIR_H
