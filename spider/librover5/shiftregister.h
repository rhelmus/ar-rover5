#ifndef SHIFTREGISTER_H
#define SHIFTREGISTER_H

#include <Arduino.h>

class CShiftRegister
{
    uint16_t bits; // Two 595 daisy chained --> 16 bits

public:
    CShiftRegister(void) : bits(0) { }

    void init(void);
    void clear(void) { bits = 0; }
    void set(uint8_t bit, uint8_t val) { bitWrite(bits, bit, val); }
    void apply(void);
};

extern CShiftRegister shiftRegister;

#endif // SHIFTREGISTER_H
