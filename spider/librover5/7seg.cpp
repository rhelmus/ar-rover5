#include "7seg.h"
#include "constants.h"

#include <Arduino.h>

C7Seg sevenSeg;

void C7Seg::drawSegments(uint8_t d)
{
    uint8_t dpin;

    switch (d)
    {
    case 0: dpin = PIN_7SEG_D1; break;
    case 1: dpin = PIN_7SEG_D2; break;
    case 2: dpin = PIN_7SEG_D3; break;
    case 3: dpin = PIN_7SEG_D4; break;
    }

    digitalWrite(PIN_7SEG_A, (digits[d].a) ? LOW : HIGH);
    digitalWrite(PIN_7SEG_B, (digits[d].b) ? LOW : HIGH);
    digitalWrite(PIN_7SEG_C, (digits[d].c) ? LOW : HIGH);
    digitalWrite(PIN_7SEG_D, (digits[d].d) ? LOW : HIGH);
    digitalWrite(PIN_7SEG_E, (digits[d].e) ? LOW : HIGH);
    digitalWrite(PIN_7SEG_F, (digits[d].f) ? LOW : HIGH);
    digitalWrite(PIN_7SEG_G, (digits[d].g) ? LOW : HIGH);
    digitalWrite(PIN_7SEG_DEC, (digits[d].dec) ? LOW : HIGH);

    digitalWrite(dpin, HIGH);
    delayMicroseconds(50);
    digitalWrite(dpin, LOW);
}

void C7Seg::init()
{
    pinMode(PIN_7SEG_A, OUTPUT);
    pinMode(PIN_7SEG_B, OUTPUT);
    pinMode(PIN_7SEG_C, OUTPUT);
    pinMode(PIN_7SEG_D, OUTPUT);
    pinMode(PIN_7SEG_E, OUTPUT);
    pinMode(PIN_7SEG_F, OUTPUT);
    pinMode(PIN_7SEG_G, OUTPUT);
    pinMode(PIN_7SEG_DEC, OUTPUT);
    pinMode(PIN_7SEG_D1, OUTPUT);
    pinMode(PIN_7SEG_D2, OUTPUT);
    pinMode(PIN_7SEG_D3, OUTPUT);
    pinMode(PIN_7SEG_D4, OUTPUT);

    digitalWrite(PIN_7SEG_D1, LOW);
    digitalWrite(PIN_7SEG_D2, LOW);
    digitalWrite(PIN_7SEG_D3, LOW);
    digitalWrite(PIN_7SEG_D4, LOW);

    clear();
}

void C7Seg::update()
{
    for (uint8_t d=0; d<4; ++d)
    {
        if (digits[d].a || digits[d].b || digits[d].c  || digits[d].d || digits[d].e ||
            digits[d].f || digits[d].g || digits[d].dec)
            drawSegments(d);
    }
}

void C7Seg::clear()
{
    memset(digits, 0, sizeof(digits));
}

void C7Seg::clearDec()
{
    for (uint8_t d=0; d<4; ++d)
        digits[d].dec = false;
}

void C7Seg::setDigit(uint8_t d, uint8_t n)
{
    if (n == 0)
    {
        digits[d].a = digits[d].b = digits[d].c = digits[d].d = true;
        digits[d].e = digits[d].f = true;
        digits[d].g = false;
    }
    else if (n == 1)
    {
        digits[d].a = digits[d].d = digits[d].e = digits[d].f = digits[d].g = false;
        digits[d].b = digits[d].c = true;
    }
    else if (n == 2)
    {
        digits[d].a = digits[d].b = digits[d].d = digits[d].e = digits[d].g = true;
        digits[d].c = digits[d].f = false;
    }
    else if (n == 3)
    {
        digits[d].a = digits[d].b = digits[d].c = digits[d].d = digits[d].g = true;
        digits[d].e = digits[d].f = false;
    }
    else if (n == 4)
    {
        digits[d].b = digits[d].c = digits[d].f = digits[d].g = true;
        digits[d].a = digits[d].d = digits[d].e = false;
    }
    else if (n == 5)
    {
        digits[d].a = digits[d].c = digits[d].d = digits[d].f = digits[d].g = true;
        digits[d].b = digits[d].e = false;
    }
    else if (n == 6)
    {
        digits[d].c = digits[d].d = digits[d].e = digits[d].f = digits[d].g = true;
        digits[d].a = digits[d].b = false;
    }
    else if (n == 7)
    {
        digits[d].a = digits[d].b = digits[d].c = true;
        digits[d].d = digits[d].e = digits[d].f = digits[d].g = false;
    }
    else if (n == 8)
    {
        digits[d].a = digits[d].b = digits[d].c = digits[d].d = true;
        digits[d].e = digits[d].f = digits[d].g = true;
    }
    else // if (n == 9)
    {
        digits[d].a = digits[d].b = digits[d].c = digits[d].f = digits[d].g = true;
        digits[d].d = digits[d].e = false;
    }
}

void C7Seg::setDec(uint8_t d)
{
    digits[d].dec = true;
}

void C7Seg::setVal(uint16_t v)
{
    clearDec();

    uint8_t d = 3;
    while (v)
    {
        setDigit(d, v % 10);
        v /= 10;
        if (v == 0)
            break;
        --d;
    }
}

void C7Seg::setVal(float v, uint8_t d)
{
    setVal(static_cast<uint16_t>(v * pow(10, d)));
    setDec(3-d);
}
