#include <ar-rover5.h>

#define CALIB_FORWARD
//#define CALIB_TURN_LEFT
//#define CALIB_TURN_LEFT


namespace {

enum
{
    SW_LOW_PIN = 27,
    SW_HIGH_PIN = 29,

    RUN_TIME = 4000,
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
    static bool dump;
    const uint32_t curtime = millis();

    if (runtime == 0)
    {
        // Switch pressed? (ie. back to unpressed)
        if (switchChanged() && (switchData.curState == HIGH))
        {
            dump = false;
            motors.enable();

#ifdef CALIB_FORWARD
            motors.move(RUN_SPEED, DIR_FWD);
#elif CALIB_TURN_LEFT
            motors.turn(RUN_SPEED, DIR_LEFT);
#elif CALIB_TURN_RIGHT
            motors.turn(RUN_SPEED, DIR_RIGHT);
#endif
            runtime = curtime + RUN_TIME;
        }
    }
    else if (runtime < curtime)
    {
        motors.stop();
        runtime = 0;
        dump = true;
    }
#if 0
    else if (speedtime < curtime)
    {
//        Serial.println("Speed:");
//        Serial.print("left-back: "); Serial.println(encoders.getSpeed(ENC_LB), DEC);
//        Serial.print("left-front: "); Serial.println(encoders.getSpeed(ENC_LF), DEC);
//        Serial.print("right-back: "); Serial.println(encoders.getSpeed(ENC_RB), DEC);
//        Serial.print("right-front: "); Serial.println(encoders.getSpeed(ENC_RF), DEC);

        Serial.print(encoders.getSpeed(ENC_LB), DEC);
        Serial.print(","); Serial.print(encoders.getSpeed(ENC_LF), DEC);
        Serial.print(","); Serial.print(encoders.getSpeed(ENC_RB), DEC);
        Serial.print(","); Serial.println(encoders.getSpeed(ENC_RF), DEC);

        speedtime = curtime + 500;
    }
#endif

    if (dump && !encoders.getSpeed(ENC_LB) && !encoders.getSpeed(ENC_LF) &&
        !encoders.getSpeed(ENC_RB) && !encoders.getSpeed(ENC_RF))
    {
        Serial.println("Run finished!");
        Serial.print("left-back: "); Serial.println(encoders.getDist(ENC_LB), DEC);
        Serial.print("left-front: "); Serial.println(encoders.getDist(ENC_LF), DEC);
        Serial.print("right-back: "); Serial.println(encoders.getDist(ENC_RB), DEC);
        Serial.print("right-front: "); Serial.println(encoders.getDist(ENC_RF), DEC);
        encoders.resetDist();
        dump = false;
    }

    rover5Task();
}
