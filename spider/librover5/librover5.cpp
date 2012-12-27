#include "librover5.h"

#include <Servo.h>
#include <TimerOne.h>


namespace {

Servo servo;
LSM303 compass;

void timerISR(void)
{
    encoders.update();
    motors.update();
}

}

void initRover5()
{
    pinMode(PIN_RELAY, OUTPUT);

#if 0
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_YELLOW, OUTPUT);

    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_YELLOW, HIGH);
#endif


    encoders.init();
    motors.init();
    sevenSeg.init();

    Timer1.initialize(200000); // Every 200 ms
    Timer1.attachInterrupt(timerISR);

    servo.attach(PIN_SERVO);
    servo.write(90);

    Wire.begin();
    compass.init();
    compass.enableDefault();
    compass.m_min.x = -601; compass.m_min.y = -558; compass.m_min.z = -630;
    compass.m_max.x = +358; compass.m_max.y = +386; compass.m_max.z = 475;
    compass.setTimeout(100);
}

void rover5Task()
{
    static uint32_t ADCCheckDelay;
    static uint8_t spos = 90;
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

#if 0
        Serial.print("Sharp IR: "); Serial.println(getSharpIRDistance(), DEC);

        Serial.print("sharp-left: "); Serial.println(analogRead(PIN_SHARP_IR_LEFT), DEC);
        Serial.print("sharp-left-fw: "); Serial.println(analogRead(PIN_SHARP_IR_LEFT_FW), DEC);
        Serial.print("sharp-fw: "); Serial.println(analogRead(PIN_SHARP_IR_FW), DEC);
        Serial.print("sharp-right: "); Serial.println(analogRead(PIN_SHARP_IR_RIGHT), DEC);
        Serial.print("sharp-right-fw: "); Serial.println(analogRead(PIN_SHARP_IR_RIGHT_FW), DEC);
#endif

        for (uint8_t i=0; i<SHARPIR_END; ++i)
        {
            Serial.print("SIR "); Serial.print(i, DEC); Serial.print(": ");
            Serial.println(sharpIR[i].getDistance());
        }

        /*servo.write(spos);
        if (spos >= 180)
            spos = 0;
        else
            spos += 15;*/

        compass.read();
        int heading = compass.heading((LSM303::vector){0,-1,0});
        Serial.println(heading);
//        sevenSeg.setVal(heading);

        if (motors.isEnabled())
        {
            const uint16_t batadc = analogRead(PIN_BATTERY);
            sevenSeg.setVal(static_cast<float>(batadc) / 1024.0 * 5.0 * 2.0, 2);
        }

        ADCCheckDelay = curtime + 400;
    }

    sevenSeg.update();
}
