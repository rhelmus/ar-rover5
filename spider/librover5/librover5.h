#ifndef AR_ROVER5
#define AR_ROVER5

#include <Arduino.h>
#include <Wire.h>

#include "../../shared/shared.h"
#include "7seg.h"
#include "button.h"
#include "constants.h"
#include "encoders.h"
#include "motors.h"
#include "remoteinterface.h"
#include "shiftregister.h"
#include "sharpir.h"

class Servo;
class LSM303;

void initRover5(void);
void rover5Task(void);
Servo &getServo(void);
LSM303 &getCompass(void);

#endif

