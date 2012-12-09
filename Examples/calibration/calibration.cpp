#include <ar-rover5.h>

//#define CALIB_FORWARD
#define CALIB_TURN_LEFT
//#define CALIB_TURN_LEFT


namespace {

enum
{
    SW_LOW_PIN = 27,
    SW_HIGH_PIN = 29,

    RUN_DIST = 70,
    RUN_ROTATION = 180,
    RUN_SPEED = 80
};

enum { MIN_SW_TIME = 50 };

static struct
{
    uint8_t curState;
    uint32_t startTime;
} switchData;


bool switchChanged(void)
{
    const uint8_t sw = digitalRead(SW_LOW_PIN);

    if (sw != switchData.curState)
    {
        const uint32_t curtime = millis();

        if (switchData.startTime == 0)
            switchData.startTime = curtime;
        else if ((curtime - switchData.startTime) > MIN_SW_TIME)
        {
            switchData.curState = sw;
            switchData.startTime = 0;
            return true;
        }
    }

    return false;
}

}


void setup()
{
    pinMode(SW_LOW_PIN, INPUT);
    digitalWrite(SW_LOW_PIN, HIGH);

    pinMode(SW_HIGH_PIN, OUTPUT);
    digitalWrite(SW_HIGH_PIN, LOW);

    Serial.begin(115200);

    initRover5();

    switchData.curState = HIGH;
    switchData.startTime = 0;

    Serial.println("Initialized");
}

void loop()
{
    static uint32_t runtime, speedtime;
    static bool running;
    const uint32_t curtime = millis();

    if (!running)
    {
        // Switch pressed? (ie. back to unpressed)
        if (switchChanged() && (switchData.curState == HIGH))
        {
            motors.enable();

#ifdef CALIB_FORWARD
            motors.moveDistCm(RUN_SPEED, RUN_DIST, DIR_FWD);
#elif defined(CALIB_TURN_LEFT)
            motors.turnAngle(RUN_SPEED, 180, DIR_LEFT);
#elif defined(CALIB_TURN_RIGHT)
            motors.turn(RUN_SPEED, DIR_RIGHT);
#endif
            running = true;
            runtime = curtime;
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
        !encoders.getSpeed(ENC_RF)*/ motors.finishedMoving())
    {
        Serial.println("Run finished!");
        Serial.print("left-back: "); Serial.println(encoders.getDist(ENC_LB), DEC);
        Serial.print("left-front: "); Serial.println(encoders.getDist(ENC_LF), DEC);
        Serial.print("right-back: "); Serial.println(encoders.getDist(ENC_RB), DEC);
        Serial.print("right-front: "); Serial.println(encoders.getDist(ENC_RF), DEC);
        encoders.resetDist();
        running = false;
    }

    rover5Task();
}
