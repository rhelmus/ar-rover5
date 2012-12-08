#ifndef ENCODERS_H
#define ENCODERS_H

#include <stdint.h>

enum EEncoder { ENC_LB=0, ENC_LF, ENC_RB, ENC_RF, ENC_COUNT };

class CEncoders
{
    volatile int16_t encSpeed[ENC_COUNT];

public:
    void init(void);
    void update(void);

    int16_t getSpeed(EEncoder e) const { return encSpeed[e]; }
    uint32_t getDist(EEncoder e) const;
    void resetDist(void);
};

extern CEncoders encoders;

#endif // ENCODERS_H
