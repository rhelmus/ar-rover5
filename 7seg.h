#ifndef SEVENSEG_H
#define SEVENSEG_H

#include <stdint.h>

class C7Seg
{
    struct SLed
    {
        uint8_t enabled : 1;
        uint8_t a : 1, b : 1, c : 1, d : 1, e : 1, f : 1, g : 1;
    };

    SLed segLEDs[4];

    void drawSegments(uint8_t d);

public:
    C7Seg(void) { }

    void init(void);
    void update(void);
    void clear(void);
    void setDigit(uint8_t d, uint8_t n);
    void setVal(uint16_t v);
};

extern C7Seg sevenSeg;

#endif // SEVENSEG_H
