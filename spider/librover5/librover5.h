#ifndef AR_ROVER5
#define AR_ROVER5

#include <Arduino.h>
#include <Wire.h>

#include "7seg.h"
#include "constants.h"
#include "encoders.h"
#include "motors.h"
#include "remoteinterface.h"
#include "sharpir.h"

class Servo;
class LSM303;

void initRover5(void);
void rover5Task(void);
Servo &getServo(void);
LSM303 &getCompass(void);

#endif

