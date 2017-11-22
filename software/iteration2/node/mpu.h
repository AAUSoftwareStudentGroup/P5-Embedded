#ifndef __MPU__
#define __MPU__

#include <Wire.h>
#include "config.h"
#include "datastructures.h"

void _mpu_setRegister(uint8_t addr, uint8_t val);
void mpu_setup();
datapoint mpu_read();

#endif