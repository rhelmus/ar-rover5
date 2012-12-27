#ifndef AR_ROVER5
#define AR_ROVER5

#include <Arduino.h>
#include <Wire.h>
#include <LSM303.h>

#include "7seg.h"
#include "constants.h"
#include "encoders.h"
#include "motors.h"
#include "sharpir.h"

void initRover5(void);
void rover5Task(void);

#endif

