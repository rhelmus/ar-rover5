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

    digitalWrite(PIN_7SEG_A, (segLEDs[d].a) ? LOW : HIGH);
    digitalWrite(PIN_7SEG_B, (segLEDs[d].b) ? LOW : HIGH);
    digitalWrite(PIN_7SEG_C, (segLEDs[d].c) ? LOW : HIGH);
    digitalWrite(PIN_7SEG_D, (segLEDs[d].d) ? LOW : HIGH);
    digitalWrite(PIN_7SEG_E, (segLEDs[d].e) ? LOW : HIGH);
    digitalWrite(PIN_7SEG_F, (segLEDs[d].f) ? LOW : HIGH);
    digitalWrite(PIN_7SEG_G, (segLEDs[d].g) ? LOW : HIGH);

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
    digitalWrite(PIN_7SEG_DEC, HIGH);

    clear();
}

void C7Seg::update()
{
    for (uint8_t d=0; d<4; ++d)
    {
        if (segLEDs[d].enabled)
            drawSegments(d);
    }
}

void C7Seg::clear()
{
    memset(segLEDs, 0, sizeof(segLEDs));
}

void C7Seg::setDigit(uint8_t d, uint8_t n)
{
    segLEDs[d].enabled = true;

    if (n == 0)
    {
        segLEDs[d].a = segLEDs[d].b = segLEDs[d].c = segLEDs[d].d = true;
        segLEDs[d].e = segLEDs[d].f = true;
        segLEDs[d].g = false;
    }
    else if (n == 1)
    {
        segLEDs[d].a = segLEDs[d].d = segLEDs[d].e = segLEDs[d].f = segLEDs[d].g = false;
        segLEDs[d].b = segLEDs[d].c = true;
    }
    else if (n == 2)
    {
        segLEDs[d].a = segLEDs[d].b = segLEDs[d].d = segLEDs[d].e = segLEDs[d].g = true;
        segLEDs[d].c = segLEDs[d].f = false;
    }
    else if (n == 3)
    {
        segLEDs[d].a = segLEDs[d].b = segLEDs[d].c = segLEDs[d].d = segLEDs[d].g = true;
        segLEDs[d].e = segLEDs[d].f = false;
    }
    else if (n == 4)
    {
        segLEDs[d].b = segLEDs[d].c = segLEDs[d].f = segLEDs[d].g = true;
        segLEDs[d].a = segLEDs[d].d = segLEDs[d].e = false;
    }
    else if (n == 5)
    {
        segLEDs[d].a = segLEDs[d].c = segLEDs[d].d = segLEDs[d].f = segLEDs[d].g = true;
        segLEDs[d].b = segLEDs[d].e = false;
    }
    else if (n == 6)
    {
        segLEDs[d].c = segLEDs[d].d = segLEDs[d].e = segLEDs[d].f = segLEDs[d].g = true;
        segLEDs[d].a = segLEDs[d].b = false;
    }
    else if (n == 7)
    {
        segLEDs[d].a = segLEDs[d].b = segLEDs[d].c = true;
        segLEDs[d].d = segLEDs[d].e = segLEDs[d].f = segLEDs[d].g = false;
    }
    else if (n == 8)
    {
        segLEDs[d].a = segLEDs[d].b = segLEDs[d].c = segLEDs[d].d = true;
        segLEDs[d].e = segLEDs[d].f = segLEDs[d].g = true;
    }
    else // if (n == 9)
    {
        segLEDs[d].a = segLEDs[d].b = segLEDs[d].c = segLEDs[d].f = segLEDs[d].g = true;
        segLEDs[d].d = segLEDs[d].e = false;
    }
}


void C7Seg::setVal(uint16_t v)
{
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
