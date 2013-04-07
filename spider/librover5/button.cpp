#include "button.h"

#include <Arduino.h>

void CButton::update()
{
    const uint8_t state = digitalRead(buttonPin);

    if (state == LOW) // Pressed?
    {
        if (startPressTime == 0)
            startPressTime = millis();
    }
    else
    {
        pressed = ((millis() - startPressTime) > PRESS_THRESHOLD);
        startPressTime = 0;
    }
}
