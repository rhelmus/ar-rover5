#include "../../shared/shared.h"
#include "librover5.h"
#include "utils.h"

#include <LSM303.h>
#include <Servo.h>
#include <TimerOne.h>


namespace {

Servo lowerServo, upperServo;
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
    digitalWrite(PIN_RELAY, HIGH);

    pinMode(PIN_FRONTLEDS, OUTPUT);

    pinMode(PIN_LED_GREEN_LEFT, OUTPUT);
    pinMode(PIN_LED_GREEN_RIGHT, OUTPUT);
    pinMode(PIN_LED_YELLOW_LEFT, OUTPUT);
    pinMode(PIN_LED_YELLOW_RIGHT, OUTPUT);
    pinMode(PIN_LED_RED_FRONT, OUTPUT);
    pinMode(PIN_LED_RED_BACK, OUTPUT);

    motors.init();
    shiftRegister.init();
    sevenSeg.init();

    Timer1.initialize(200000); // Every 200 ms
    Timer1.attachInterrupt(timerISR);

    lowerServo.attach(PIN_LOWERSERVO);
    lowerServo.write(90);

    upperServo.attach(PIN_UPPERSERVO);
    upperServo.write(135);

    // From http://arduino.cc/en/Reference/Random
    randomSeed(analogRead(PIN_RANDOM));

    remoteInterface.init();

    Wire.begin(SPIDER_TWI_ADDRESS);

#if 1
    compass.init();
    compass.enableDefault();
    compass.setMagGain(LSM303::magGain_56);
#if 0
    // Default gain
    compass.m_min.x = -647; compass.m_min.y = -152; compass.m_min.z = -324;
    compass.m_max.x = 347; compass.m_max.y = 752; compass.m_max.z = 583;
#endif

    // Gain 5.6
    compass.m_min.x = -173; compass.m_min.y = -2; compass.m_min.z = -84;
    compass.m_max.x = 96; compass.m_max.y = 226; compass.m_max.z = 182;

    compass.setTimeout(100);
#endif

    encoders.init(); // Need to do this after compass (uses it)
}

void rover5Task()
{
    static uint32_t ADCCheckDelay, compassreaddelay;
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

        /*for (uint8_t e=0; e<ENC_END; ++e)
        {
            const uint16_t espeed = abs(encoders.getSpeed((EEncoder)e));
            const int16_t diff = motors.getTargetSpeed((EMotor)e) - espeed;
            Serial.print("enc speed "); Serial.print(e, DEC);
            Serial.print(": "); Serial.println(diff);
        }*/

        /*servo.write(spos);
        if (spos >= 180)
            spos = 0;
        else
            spos += 15;*/

        //if (motors.isEnabled())
        {
            const uint16_t batadc = analogRead(PIN_BATTERY);
            sevenSeg.setVal(static_cast<float>(batadc) / 1024.0 * 5.0 * 2.0, 2);
        }

        ADCCheckDelay = curtime + 400;
    }
#if 1
    if (compassreaddelay < curtime)
    {
        static uint8_t dumpcounter;
        static CRollingAverage<6, uint16_t> magreadings;

        compassreaddelay = curtime + 250;

        const int16_t oldax = compass.a.x;
        const int16_t olday = compass.a.y;
        const int16_t oldaz = compass.a.z;

        compass.read();

        const int16_t dax = compass.a.x - oldax;
        const int16_t day = compass.a.y - olday;
        const int16_t daz = compass.a.z - oldaz;
        const int16_t magnitude = sqrt(sq(compass.a.x) + sq(compass.a.y) + sq(compass.a.z)); //sqrt(sq(dax) + sq(day) + sq(daz));

        magreadings.add(magnitude);
        ++dumpcounter;

        if (dumpcounter > 10)
        {
            //Serial.print("delta accel x, y, z: ");
//            Serial.print(dax); Serial.print(",");
//            Serial.print(day); Serial.print(",");
//            Serial.print(daz); Serial.print(",");
            Serial.println(magreadings.average());
            dumpcounter = 0;
        }

        /*if (magnitude > 150)
        {
            Serial.print("magnitude: "); Serial.println(magnitude);
            Serial.print("angle: "); Serial.println((atan2(compass.a.y, compass.a.x) * 180.0 / PI) + 180);
        }*/
    }
#endif
    sevenSeg.update();
    remoteInterface.update();

    for (uint8_t i=0; i<SHARPIR_END; ++i)
    {
        sharpIR[i].update();

        // UNDONE!!
        /*if (sharpIR[i].getTotReadingCount() > 5)
            sharpIR[i].clearReadings();*/
    }
}

Servo &getLowerServo()
{
    return lowerServo;
}

LSM303 &getCompass()
{
    return compass;
}
