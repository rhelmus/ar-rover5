// SIR code based on
// http://tthheessiiss.wordpress.com/2009/08/05/dirt-cheap-wireless/

#include <SoftwareSerial.h>

#include "IRcontrol.h"

namespace {

enum EADCButtonValues
{
    BUTTON_THRESHOLD = 100, // Minimal ADC value for button press

    BUTTON_LEFT = 490,
    BUTTON_RIGHT = 865,
    BUTTON_UP = 511,
    BUTTON_DOWN = 749,
    BUTTON_LEFT_UP = 808,
    BUTTON_LEFT_DOWN = 804,
    BUTTON_RIGHT_UP = 886,
    BUTTON_RIGHT_DOWN = 864,
    BUTTON_MODIFIER = 946,
    BUTTON_MODIFIER_UP = 952,
    BUTTON_MODIFIER_DOWN = 959
};

SoftwareSerial sserial(0, 3, true); // RX, TX, inverted
bool translating, translatingPressed;
uint8_t moveSpeed = 7;

bool isPressed(uint16_t butval, EADCButtonValues button)
{
    const uint8_t range = 2;
    return ((butval >= ((uint16_t)button-range)) && (butval <= ((uint16_t)button+range)));
}

EIRCommand getCommand(uint16_t butval)
{
    if (isPressed(butval, BUTTON_UP))
        return IRCMD_FWD;
    if (isPressed(butval, BUTTON_DOWN))
        return IRCMD_BWD;
    if (isPressed(butval, BUTTON_LEFT))
        return IRCMD_LEFT;
    if (isPressed(butval, BUTTON_RIGHT))
        return IRCMD_RIGHT;
    if (isPressed(butval, BUTTON_LEFT_UP))
        return IRCMD_LEFT_FWD;
    if (isPressed(butval, BUTTON_LEFT_DOWN))
        return IRCMD_LEFT_BWD;
    if (isPressed(butval, BUTTON_RIGHT_UP))
        return IRCMD_RIGHT_FWD;
    if (isPressed(butval, BUTTON_RIGHT_DOWN))
        return IRCMD_RIGHT_BWD;

    return IRCMD_NONE;
}

void updateSpeedLED(void)
{
//    analogWrite(4, moveSpeed * 16);
    OCR1B = (moveSpeed * 16);

//    bitWrite(GTCCR, PWM1B, 1);
//    GTCCR |= 2<<COM1B0;
}

}

void setup()  
{ 
    sserial.begin(1200);
    
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);
    delay(1000);
    digitalWrite(4, LOW);
    
    // IR pulse train
    pinMode(1, OUTPUT);
    bitWrite(TCCR1, CTC1, 1); // CTC
    bitWrite(TCCR1, COM1A0, 1); // Toggle output
    bitWrite(TCCR1, CS10, 1); // Prescaling = 1
    OCR1A = 110; // Approx 72 kHz, so we get 36 kHz pulses

    // Speed LED PWM
//    bitWrite(GTCCR, COM1B0, 1); // Toggle output
    bitWrite(GTCCR, PWM1B, 1);
    bitWrite(GTCCR, COM1B1, 1);
    OCR1C = 255;

//    bitWrite(GTCCR, COM1B0, 1);

    updateSpeedLED();
}

void loop()
{
    static uint32_t updelay;
    const uint32_t curtime = millis();
    
    if (updelay < curtime)
    {
        updelay = curtime + 50;
        const uint16_t butval = analogRead(1);
        /*uint16_t butval = 0;
        for (uint8_t i=0; i<3; ++i)
        {
            butval += analogRead(1);
            delay(5);
        }
        butval /= 3;*/

        if (butval > BUTTON_THRESHOLD)
        {
#if 1
            if (isPressed(butval, BUTTON_MODIFIER))
                translatingPressed = true;
            else if (isPressed(butval, BUTTON_MODIFIER_UP))
            {
                translatingPressed = false;
                if (moveSpeed < 15)
                    ++moveSpeed;
                updateSpeedLED();
            }
            else if (isPressed(butval, BUTTON_MODIFIER_DOWN))
            {
                translatingPressed = false;
                if (moveSpeed > 0)
                    --moveSpeed;
                updateSpeedLED();
            }
            else // Move
            {
                const EIRCommand cmd = getCommand(butval);

                if (cmd != IRCMD_NONE)
                {
                    const SIRMessage msg(getCommand(butval), moveSpeed, translating);
                    const uint8_t smsg[] = { msg.byte, IRCheckSum(msg) };
                    sserial.write(smsg, 2);
                }
            }
#else
            sserial.println(butval, DEC);
#endif
        }
        else if (translatingPressed)
        {
            translatingPressed = false;
            translating = !translating;
        }
    }
}

