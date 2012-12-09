#ifndef CONSTANTS_H
#define CONSTANTS_H

enum
{
    // Motors
    PIN_MOTOR_LB_PWM = 5,
    PIN_MOTOR_LB_DIR = 6,
    PIN_MOTOR_LB_CUR = 2,

    PIN_MOTOR_LF_PWM = 3,
    PIN_MOTOR_LF_DIR = 4,
    PIN_MOTOR_LF_CUR = 3,

    PIN_MOTOR_RB_PWM = 9,
    PIN_MOTOR_RB_DIR = 10,
    PIN_MOTOR_RB_CUR = 0,

    PIN_MOTOR_RF_PWM = 7,
    PIN_MOTOR_RF_DIR = 8,
    PIN_MOTOR_RF_CUR = 1,

    // Encoders
    PIN_ENC_LB_SIGN1 = 33,
    PIN_ENC_LB_SIGN2 = 34,
    INT_ENC_LB = 5,

    PIN_ENC_LF_SIGN1 = 31,
    PIN_ENC_LF_SIGN2 = 32,
    INT_ENC_LF = 4,

    PIN_ENC_RB_SIGN1 = 37,
    PIN_ENC_RB_SIGN2 = 38,
    INT_ENC_RB = 0,

    PIN_ENC_RF_SIGN1 = 35,
    PIN_ENC_RF_SIGN2 = 36,
    INT_ENC_RF = 1,

    // Misc
    PIN_RELAY = 30
};

// Movement
#define TRACKED_MOVEMENT
#define ENC_PULSES_CM 18.28
#define ENC_PULSES_DEG 5.62

#endif // CONSTANTS_H
