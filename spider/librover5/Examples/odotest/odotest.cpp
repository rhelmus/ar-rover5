#include <Wire.h>
#include <LSM303.h>
#include <Servo.h>
#include <TimerOne.h>

#include <librover5.h>

// Odometry positioning from http://geology.heroy.smu.edu/~dpa-www/robo/Encoder/imu_odo/

void setup()
{
    Serial.begin(115200);

    initRover5();   
    motors.enable();

    Serial.println("Initialized");
}

void loop()
{
    static uint32_t updelay;
    static float xpos, ypos, theta;
    static int32_t lastlenc, lastrenc;
    const uint32_t curtime = millis();
    static const float PI2 = PI * 2.0;

    if (updelay < curtime)
    {
        if (updelay == 0) // Init
        {
            getCompass().read();
            theta = (getCompass().heading() * PI / 180.0);
        }

        updelay = curtime + 500;

        const int32_t le = ((encoders.getDist(ENC_LB) + encoders.getDist(ENC_LF)) / 2);
        const int32_t re = ((encoders.getDist(ENC_RB) + encoders.getDist(ENC_RF)) / 2);

        const int32_t lticks = le - lastlenc, rticks = re - lastrenc;

        lastlenc = le;
        lastrenc = re;

        const float ldist = lticks / ENC_PULSES_CM, rdist = rticks / ENC_PULSES_CM;

        theta += (((float)(lticks - rticks) / ENC_PULSES_DEG / 2.0) * PI / 180.0);

        // Clamp
        while (true)
        {
            if (theta > PI2)
                theta -= PI2;
            else if (theta < 0)
                theta += PI2;
            else
                break;
        }

//        theta -= ((float)((int)(theta/M_2_PI)) * M_2_PI);

        const float dist = (ldist + rdist) / 2.0;
        xpos += dist * 1.0;//sin(theta);
        ypos += dist * 1.0;//cos(theta);

        Serial.print("X/Y/theta: ");
        Serial.print(xpos); Serial.print(", ");
        Serial.print(ypos); Serial.print(", ");
        Serial.println(theta * 180.0 / PI);
    }

    rover5Task();
}
