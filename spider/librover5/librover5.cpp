#include "../../shared/shared.h"
#include "librover5.h"

#include <LSM303.h>
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
    digitalWrite(PIN_RELAY, HIGH);

    encoders.init();
    motors.init();
    shiftRegister.init();
    sevenSeg.init();

    Timer1.initialize(200000); // Every 200 ms
    Timer1.attachInterrupt(timerISR);

    servo.attach(PIN_SERVO);
    servo.write(90);

    // Also initializes Wire
    remoteInterface.init();

#if 1
    compass.init();
    compass.enableDefault();
    compass.m_min.x = -414; compass.m_min.y = -809; compass.m_min.z = -391;
    compass.m_max.x = 520; compass.m_max.y = 212; compass.m_max.z = 632;
    compass.setTimeout(100);
#endif
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

        /*servo.write(spos);
        if (spos >= 180)
            spos = 0;
        else
            spos += 15;*/

        if (motors.isEnabled())
        {
            const uint16_t batadc = analogRead(PIN_BATTERY);
            sevenSeg.setVal(static_cast<float>(batadc) / 1024.0 * 5.0 * 2.0, 2);
        }

        ADCCheckDelay = curtime + 400;
    }

    sevenSeg.update();
    remoteInterface.update();
}

Servo &getServo()
{
    return servo;
}

LSM303 &getCompass()
{
    return compass;
}
