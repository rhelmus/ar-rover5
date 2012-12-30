#ifndef SHARED_H
#define SHARED_H

#include <stdint.h>

enum EMessage
{
    // Phone messages
    MSG_CAMFRAME = 0,

    // Spider messages
    MSG_PING,
    MSG_SHARPIR,
    MSG_MOTOR_TARGETPOWER,
    MSG_MOTOR_SETPOWER,
    MSG_MOTOR_TARGETSPEED,
    MSG_MOTOR_TARGETDIST,
    MSG_MOTOR_CURRENT,
    MSG_ENCODER_SPEED,
    MSG_ENCODER_DISTANCE,
    MSG_SERVO,
    MSG_BATTERY,
    MSG_HEADING,

    MSG_NONE,

    MSG_BT_STARTMARKER,
    MSG_BT_ENDMARKER,

    // Range to pass through unchanged from spider to controller
    MSG_PASS_START = MSG_SHARPIR,
    MSG_PASS_END = MSG_HEADING
};

enum
{
    BRIDGE_TWI_ADDRESS = 10,
    BRIDGE_PING_TIMEOUT = 500 // ping time (ms) should be less for active connection
};

enum EMotor
{
    MOTOR_LB=0,
    MOTOR_LF,
    MOTOR_RB,
    MOTOR_RF,
    MOTOR_END
};

enum EEncoder { ENC_LB=0, ENC_LF, ENC_RB, ENC_RF, ENC_END };

enum
{
    SHARPIR_TURRET = 0,
    SHARPIR_LEFT,
    SHARPIR_LEFT_FW,
    SHARPIR_FW,
    SHARPIR_RIGHT,
    SHARPIR_RIGHT_FW,
    SHARPIR_END
};

inline uint16_t bytesToInt(uint8_t low, uint8_t high)
{
    return static_cast<uint16_t>(low | (high << 8));
}

inline uint32_t bytesToLong(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
    return (static_cast<uint32_t>(b4) << 24) | (static_cast<uint32_t>(b3) << 16) |
           (static_cast<uint32_t>(b2) << 8) | b1;
}

inline void intToBytes(uint16_t i, uint8_t *buf)
{
    buf[0] = i;
    buf[1] = (i >> 8);
}

inline void longToBytes(uint32_t i, uint8_t *buf)
{
    buf[0] = i;
    buf[1] = (i >> 8);
    buf[2] = (i >> 16);
    buf[3] = (i >> 24);
}

#endif // SHARED_H
