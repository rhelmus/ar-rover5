#include "ar-rover5.h"
#include "constants.h"

#include <TimerFive.h>

namespace {

void timer5ISR(void)
{
    encoders.update();
    motors.update();
}

}

void initRover5()
{
    pinMode(PIN_RELAY, OUTPUT);

    encoders.init();
    motors.init();

    Timer5.initialize(200000); // Every 200 ms
    Timer5.attachInterrupt(timer5ISR);
}

void rover5Task()
{
    static uint32_t ADCCheckDelay;
    const uint32_t curtime = millis();

    if (curtime > ADCCheckDelay)
    {
        const uint16_t curLB = motors.getCurrent(MOTOR_LB);
        const uint16_t curLF = motors.getCurrent(MOTOR_LF);
        const uint16_t curRB = motors.getCurrent(MOTOR_RB);
        const uint16_t curRF = motors.getCurrent(MOTOR_RF);

        if ((curLB > 512) || (curLF > 512) || (curRB > 512) || (curRF > 512))
        {
            motors.directStop();

            Serial.println("Motor over current!!");
            Serial.print("left-back: "); Serial.println(curLB, DEC);
            Serial.print("left-front: "); Serial.println(curLF, DEC);
            Serial.print("right-back: "); Serial.println(curRB, DEC);
            Serial.print("right-front: "); Serial.println(curRF, DEC);

            // UNDONE
            /*while (true)
                ;*/
        }

        /*Serial.print("left-back: "); Serial.println(curLB, DEC);
        Serial.print("left-front: "); Serial.println(curLF, DEC);
        Serial.print("right-back: "); Serial.println(curRB, DEC);
        Serial.print("right-front: "); Serial.println(curRF, DEC);*/

        ADCCheckDelay = curtime + 400;
    }
}
