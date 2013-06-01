#include <Wire.h>
#include <LSM303.h>
#include <Servo.h>
#include <TimerOne.h>

#include <librover5.h>

// Based on calibration example from LSM303 library

namespace {

LSM303::vector running_min = { 2047, 2047, 2047 }, running_max = { -2048, -2048, -2048 };

}

void setup()
{
    Serial.begin(115200);

    initRover5();   

    Serial.println("Initialized");
}

void loop()
{
    static uint32_t updelay;
    const uint32_t curtime = millis();

    if (updelay < curtime)
    {
        updelay = curtime + 100;

        getCompass().read();

        running_min.x = min(running_min.x, getCompass().m.x);
        running_min.y = min(running_min.y, getCompass().m.y);
        running_min.z = min(running_min.z, getCompass().m.z);

        running_max.x = max(running_max.x, getCompass().m.x);
        running_max.y = max(running_max.y, getCompass().m.y);
        running_max.z = max(running_max.z, getCompass().m.z);

        Serial.print("M min ");
        Serial.print("X: ");
        Serial.print((int)running_min.x);
        Serial.print(" Y: ");
        Serial.print((int)running_min.y);
        Serial.print(" Z: ");
        Serial.print((int)running_min.z);

        Serial.print(" M max ");
        Serial.print("X: ");
        Serial.print((int)running_max.x);
        Serial.print(" Y: ");
        Serial.print((int)running_max.y);
        Serial.print(" Z: ");
        Serial.println((int)running_max.z);
    }

    rover5Task();
}
