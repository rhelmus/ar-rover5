#include "robotai.h"

#include <librover5.h>

#include <Wire.h>
#include <LSM303.h>
#include <Servo.h>
#include <TimerOne.h>

namespace {

CButton runButton(PIN_SWITCH);
CRobotAI robotAI;

}


void setup()
{
    Serial.begin(115200);

    initRover5();
    motors.enable();

    Serial.println("Initialized");
}

void loop()
{
    static bool running;
    static uint32_t loopdelay;
    const uint32_t curtime = millis();

    if (loopdelay < curtime)
    {
        runButton.update();

        if (runButton.wasPressed())
        {
            runButton.confirmPressed();

            running = !running;

            if (running)
                robotAI.init();
            else
                robotAI.stop();
        }

        if (running)
            robotAI.think();
    }

    rover5Task();
}
