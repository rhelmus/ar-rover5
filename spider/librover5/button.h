#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

class CButton
{
    enum { PRESS_THRESHOLD = 50 };

    const uint8_t buttonPin;
    uint32_t startPressTime;
    bool pressed;

public:
    CButton(uint8_t p) : buttonPin(p), startPressTime(0), pressed(false) { }

    void update(void);
    bool wasPressed(void) const { return pressed; }
    void confirmPressed(void) { pressed = false; }
};

#endif // BUTTON_H
