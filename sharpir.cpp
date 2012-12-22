#include "constants.h"
#include "sharpir.h"

#include <Arduino.h>

CSharpIR sharpIR[SHARPIR_END] =
{
    CSharpIR(PIN_SHARP_IR_TURRET, CSharpIR::GP2Y0A02YK),
    CSharpIR(PIN_SHARP_IR_LEFT, CSharpIR::GP2Y0A21YK),
    CSharpIR(PIN_SHARP_IR_LEFT_FW, CSharpIR::GP2Y0A21YK),
    CSharpIR(PIN_SHARP_IR_FW, CSharpIR::GP2Y0A21YK),
    CSharpIR(PIN_SHARP_IR_RIGHT, CSharpIR::GP2Y0A21YK),
    CSharpIR(PIN_SHARP_IR_RIGHT_FW, CSharpIR::GP2Y0A21YK)
};


uint8_t CSharpIR::getDistance() const
{
    const uint16_t adc = analogRead(ADCPin);

    // R = (1 / (m * V + b)) - k, see http://www.acroname.com/robotics/info/articles/irlinear/irlinear.html

    if (model == GP2Y0A21YK)
        return (1 / (GP2Y0A21YK_A * adc + GP2Y0A21YK_B)) - GP2Y0A21YK_K;

    return (1 / (GP2Y0A02YK_A * adc + GP2Y0A02YK_B)) - GP2Y0A02YK_K;
}
