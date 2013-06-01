#include <Wire.h>
#include <LSM303.h>
#include <Servo.h>
#include <TimerOne.h>

#include <librover5.h>

//#define CALIB_FORWARD
#define CALIB_TURN_LEFT
//#define CALIB_TURN_RIGHT


namespace {

enum
{
    RUN_DIST = 70,
    RUN_ROTATION = 180,
    RUN_SPEED = 80
};

CButton runButton(PIN_SWITCH);

}


void setup()
{
    Serial.begin(115200);

    initRover5();
    runButton.init();

    Serial.println("Initialized");
}

void loop()
{
    static uint32_t runtime, speedtime;
    static bool running;
    const uint32_t curtime = millis();

    if (!running)
    {
        runButton.update();

        if (runButton.wasPressed())
        {
            runButton.confirmPressed();
            motors.enable();
#if 1
#ifdef CALIB_FORWARD
            motors.moveDistCm(RUN_SPEED, RUN_DIST, MDIR_FWD);
#elif defined(CALIB_TURN_LEFT)
            //motors.turnAngle(RUN_SPEED, 180, TDIR_LEFT);
#elif defined(CALIB_TURN_RIGHT)
            motors.turn(RUN_SPEED, DIR_RIGHT);
#endif
            running = true;
            runtime = curtime;
#endif
        }
    }
    else if (speedtime < curtime)
    {
        Serial.println("Speed:");
        Serial.print("left-back: "); Serial.println(encoders.getSpeed(ENC_LB), DEC);
        Serial.print("left-front: "); Serial.println(encoders.getSpeed(ENC_LF), DEC);
        Serial.print("right-back: "); Serial.println(encoders.getSpeed(ENC_RB), DEC);
        Serial.print("right-front: "); Serial.println(encoders.getSpeed(ENC_RF), DEC);
#if 0
        Serial.print(encoders.getSpeed(ENC_LB), DEC);
        Serial.print(","); Serial.print(encoders.getSpeed(ENC_LF), DEC);
        Serial.print(","); Serial.print(encoders.getSpeed(ENC_RB), DEC);
        Serial.print(","); Serial.println(encoders.getSpeed(ENC_RF), DEC);
#endif
        Serial.println("Power:");
        Serial.print("left-back: "); Serial.println(motors.getTargetPower(MOTOR_LB), DEC);
        Serial.print("left-front: "); Serial.println(motors.getTargetPower(MOTOR_LF), DEC);
        Serial.print("right-back: "); Serial.println(motors.getTargetPower(MOTOR_RB), DEC);
        Serial.print("right-front: "); Serial.println(motors.getTargetPower(MOTOR_RF), DEC);
        speedtime = curtime + 500;
    }

#if 0
    if (((encoders.getDist(ENC_LB) + encoders.getDist(ENC_LF)) / 2) >= rundistl)
    {
        Serial.print("Stopping left after: ");
        Serial.println(((encoders.getDist(ENC_LB) + encoders.getDist(ENC_LF)) / 2), DEC);
        motors.setLeftSpeed(0);
        dump = true;
    }
    if (((encoders.getDist(ENC_RB) + encoders.getDist(ENC_RF)) / 2) >= rundistr)
    {
        Serial.print("Stopping right after: ");
        Serial.println(((encoders.getDist(ENC_RB) + encoders.getDist(ENC_RF)) / 2), DEC);
        motors.setRightSpeed(0);
        dump = true;
    }
#endif
    if (running && /*((curtime - runtime) > 1000) && !encoders.getSpeed(ENC_LB) &&
        !encoders.getSpeed(ENC_LF) && !encoders.getSpeed(ENC_RB) &&
        !encoders.getSpeed(ENC_RF)*/ motors.distanceReached())
    {
        Serial.println("Run finished!");
        Serial.print("left-back: "); Serial.println(encoders.getAbsDist(ENC_LB), DEC);
        Serial.print("left-front: "); Serial.println(encoders.getAbsDist(ENC_LF), DEC);
        Serial.print("right-back: "); Serial.println(encoders.getAbsDist(ENC_RB), DEC);
        Serial.print("right-front: "); Serial.println(encoders.getAbsDist(ENC_RF), DEC);
        encoders.resetDist();
        running = false;
    }

    rover5Task();
}
