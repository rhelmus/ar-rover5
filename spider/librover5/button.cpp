#include "button.h"

#include <Arduino.h>

void CButton::init()
{
    pinMode(buttonPin, INPUT);
    digitalWrite(buttonPin, HIGH);
}

void CButton::update()
{
    const uint8_t state = digitalRead(buttonPin);

    if (state == LOW) // Pressed?
    {
        if (startPressTime == 0)
            startPressTime = millis();
    }
    else if (startPressTime != 0)
    {
        pressed = ((millis() - startPressTime) > PRESS_THRESHOLD);
        startPressTime = 0;
    }
}
