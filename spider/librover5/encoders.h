#ifndef ENCODERS_H
#define ENCODERS_H

#include "../../shared/shared.h"

#include <stdint.h>

class CEncoders
{
    volatile int16_t encSpeed[ENC_END];

public:
    void init(void);
    void update(void);

    int16_t getSpeed(EEncoder e) const { return encSpeed[e]; }
    bool isMoving(void) const;
    uint32_t getDist(EEncoder e) const;
    void resetDist(void);
};

extern CEncoders encoders;

#endif // ENCODERS_H
