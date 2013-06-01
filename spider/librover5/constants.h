#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "../../shared/shared.h"

enum
{
    // Motors
    PIN_MOTOR_LB_PWM = 5,
    PIN_MOTOR_LB_DIR = 6,
    PIN_MOTOR_LB_CUR = 2,

    PIN_MOTOR_LF_PWM = 4,
    PIN_MOTOR_LF_DIR = 11,
    PIN_MOTOR_LF_CUR = 3,

    PIN_MOTOR_RB_PWM = 9,
    PIN_MOTOR_RB_DIR = 12,
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

    PIN_ENC_RB_SIGN1 = 35,
    PIN_ENC_RB_SIGN2 = 36,
    INT_ENC_RB = 0,

    PIN_ENC_RF_SIGN1 = 37,
    PIN_ENC_RF_SIGN2 = 38,
    INT_ENC_RF = 1,

    // 595 shift registers
    PIN_SHIFTREG_SER = 39,
    PIN_SHIFTREG_RCLK = 40,
    PIN_SHIFTREG_SRCLK = 41,

    // 4 digit 7 segment display (NOTE: connected to shift registers)
    PIN_7SEG_D1 = 9,
    PIN_7SEG_D2 = 10,
    PIN_7SEG_D3 = 11,
    PIN_7SEG_D4 = 12,
    PIN_7SEG_A = 7,
    PIN_7SEG_B = 4,
    PIN_7SEG_C = 6,
    PIN_7SEG_D = 1,
    PIN_7SEG_E = 2,
    PIN_7SEG_F = 0,
    PIN_7SEG_G = 5,
    PIN_7SEG_DEC = 3,

    // Misc
    PIN_RELAY = 30,
    PIN_BATTERY = 4,
    PIN_SHARP_IR_TURRET = 5,
    PIN_SHARP_IR_LEFT = 6,
    PIN_SHARP_IR_LEFT_FW = 8,
    PIN_SHARP_IR_FW = 9,
    PIN_SHARP_IR_RIGHT = 11,
    PIN_SHARP_IR_RIGHT_FW = 12,
    PIN_LDR = 13,
    PIN_RANDOM = 15,
    PIN_SERVO = 51,
    PIN_SWITCH = 22,
    PIN_FRONTLEDS = 10
};

// Movement
#ifdef MECANUM_MOVEMENT
#define ENC_PULSES_CM 20.18
#define ENC_PULSES_DEG 6.93
#else
#define ENC_PULSES_CM 18.28
#define ENC_PULSES_DEG 5.62
#endif

// Sharp IR
// Constants derived from methodology described at http://www.acroname.com/robotics/info/articles/irlinear/irlinear.html
#define GP2Y0A21YK_K 2.5
#define GP2Y0A21YK_A 0.0001746376
#define GP2Y0A21YK_B -0.0017243507
#define GP2Y0A02YK_K 13.5
#define GP2Y0A02YK_A 5.32761917908897E-005
#define GP2Y0A02YK_B 0.001758403

#endif // CONSTANTS_H
