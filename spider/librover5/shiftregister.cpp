#include "constants.h"
#include "shiftregister.h"

CShiftRegister shiftRegister;


void CShiftRegister::init()
{
    pinMode(PIN_SHIFTREG_SER, OUTPUT);
    pinMode(PIN_SHIFTREG_RCLK, OUTPUT);
    pinMode(PIN_SHIFTREG_SRCLK, OUTPUT);

    apply(); // Start with every bit set to LOW
}

void CShiftRegister::apply()
{
    digitalWrite(PIN_SHIFTREG_RCLK, LOW); // Start transfer

    // Send pin data to both registers
    shiftOut(PIN_SHIFTREG_SER, PIN_SHIFTREG_SRCLK, MSBFIRST, highByte(bits));
    shiftOut(PIN_SHIFTREG_SER, PIN_SHIFTREG_SRCLK, MSBFIRST, lowByte(bits));

    digitalWrite(PIN_SHIFTREG_RCLK, HIGH); // Apply
}
