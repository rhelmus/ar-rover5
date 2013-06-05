#ifndef ENCODERS_H
#define ENCODERS_H

#include "../../shared/shared.h"

#include <Arduino.h>

#include <stdint.h>

class CEncoders
{
    volatile int16_t encSpeed[ENC_END];
    volatile float xPos, yPos, theta;

public:
    void init(void);
    void update(void);

    int16_t getSpeed(EEncoder e) const { return encSpeed[e]; }
    bool isMoving(void) const;
    uint32_t getAbsDist(EEncoder e) const;
    int32_t getDist(EEncoder e) const;
    void resetDist(void);
    float getXPos(void) const { return xPos; }
    float getYPos(void) const { return yPos; }
    float getRotation(void) const { return theta * 180.0 / PI; }
};

extern CEncoders encoders;

#endif // ENCODERS_H
