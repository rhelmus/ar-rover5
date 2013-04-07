#include "robotai.h"

#include <librover5.h>

void CRobotAI::setState(EState s)
{
    state = s;

    if (s == STATE_CRUISE)
        motors.move(80, MDIR_FWD);
}

void CRobotAI::init()
{
    state = STATE_CRUISE;
    checkSharpIRDelay = 0;
}

void CRobotAI::stop()
{
    motors.stop();
}

void CRobotAI::think()
{
    const uint32_t curtime = millis();

    if (state == STATE_CRUISE)
    {
        if (checkSharpIRDelay < curtime)
        {
            const uint8_t turretd = sharpIR[SHARPIR_TURRET].getDistance();
            const uint8_t fwd = sharpIR[SHARPIR_FW].getDistance();
            const uint8_t lfwd = sharpIR[SHARPIR_LEFT_FW].getDistance();
            const uint8_t rfwd = sharpIR[SHARPIR_RIGHT_FW].getDistance();

            if (((turretd >= 20) && (turretd < 40)) || ((fwd >= 15) && (fwd < 30)) ||
                ((lfwd >= 15) && (lfwd < 20)) ||((rfwd >= 15) && (rfwd < 20)))
            {
                motors.turn(80, 180, TURN_LEFT);
                state = STATE_TURN;
            }
        }
    }
}
