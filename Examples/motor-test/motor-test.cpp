#include <ar-rover5.h>

namespace {

enum
{
    SW_LOW_PIN = 27,
    SW_HIGH_PIN = 29
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
    static uint32_t updelay;
    static EMotorDirection ldir = DIR_FWD, rdir = DIR_BWD;
    static bool running = false;

    // Switch pressed? (ie. back to unpressed)
    if (switchChanged() && (switchData.curState == HIGH))
    {
        Serial.print("Changed SW:"); Serial.println(switchData.curState, DEC);

        if (running)
        {
            motors.disable();
            running = false;
        }
        else
        {
            motors.enable();
            motors.setLeftSpeed(80);
            motors.setRightSpeed(80);
            running = true;
        }
    }

    if (running)
    {
        const uint32_t curtime = millis();

        if (updelay < curtime)
        {
            updelay = curtime + 7500;
            //        motors.move(80, mdir);
            motors.setLeftDirection(ldir);
            motors.setRightDirection(/*rdir*/ldir);

            if (ldir == DIR_FWD)
            {
                ldir = DIR_BWD;
                rdir = DIR_FWD;
            }
            else
            {
                ldir = DIR_FWD;
                rdir = DIR_BWD;
            }
        }
    }

    rover5Task();
}
