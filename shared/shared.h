#ifndef SHARED_H
#define SHARED_H

#include <stdint.h>

enum EMessage
{
    // Spider to control messages
    MSG_SHARPIR = 0,
    MSG_MOTOR_TARGETPOWER,
    MSG_MOTOR_SETPOWER,
    MSG_MOTOR_TARGETSPEED,
    MSG_MOTOR_TARGETDIST,
    MSG_MOTOR_CURRENT,
    MSG_ENCODER_SPEED,
    MSG_ENCODER_DISTANCE,
    MSG_ODOMETRY,
    MSG_SERVO,
    MSG_BATTERY,
    MSG_IMU,

    // Control to spider messages
    MSG_CMD_MOTORSPEED,
    MSG_CMD_DRIVEDIST,
    MSG_CMD_TURN,
    MSG_CMD_TURNANGLE,
    MSG_CMD_TRANSLATE,
    MSG_CMD_TRANSLATEDIST,
    MSG_CMD_STOP,
    MSG_CMD_FRONTLEDS,

    // Messages between phone and control
    MSG_CAMFRAME,
    MSG_SETZOOM,

    // Messages between bridge and control
    MSG_PING, // Ping from bridge to control
    MSG_PONG, // Ping response from control to bridge
    MSG_CNTRL_DISCONNECT,

    // Bluetooth specific
    MSG_BT_STARTMARKER,
    MSG_BT_ENDMARKER,

    MSG_NONE,

    // Range to pass through unchanged from spider to controller
    MSG_PASS_START = MSG_SHARPIR,
    MSG_PASS_END = MSG_IMU
};

enum
{
    SPIDER_TWI_ADDRESS = 10,
    BRIDGE_TWI_ADDRESS = 11,
    BRIDGE_PING_TIMEOUT = 500, // ping time (ms) should be less for active connection
    BRIDGE_MAX_REQSIZE = 16
};

#define TRACKED_MOVEMENT
//#define MECANUM_MOVEMENT

enum
{
#ifdef MECANUM_MOVEMENT
    MIN_MOTOR_POWER = 20,
    MAX_MOTOR_POWER = 200
#else
    MIN_MOTOR_POWER = 50,
    MAX_MOTOR_POWER = 160
#endif
};

enum EMotor
{
    MOTOR_LB=0,
    MOTOR_LF,
    MOTOR_RB,
    MOTOR_RF,
    MOTOR_END
};

enum EMotorDirection { MDIR_FWD, MDIR_BWD };
enum ETurnDirection { TDIR_LEFT, TDIR_RIGHT };

#ifdef MECANUM_MOVEMENT
enum ETranslateDirection
{
    TRDIR_LEFT,
    TRDIR_LEFT_FWD,
    TRDIR_LEFT_BWD,
    TRDIR_RIGHT,
    TRDIR_RIGHT_FWD,
    TRDIR_RIGHT_BWD
};
#endif

enum EEncoder { ENC_LB=0, ENC_LF, ENC_RB, ENC_RF, ENC_END };

enum ESharpIRSensor
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

inline float bytesToFloat(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
    return static_cast<float>((int32_t)bytesToLong(b1, b2, b3, b4)) / 1000.0;
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
