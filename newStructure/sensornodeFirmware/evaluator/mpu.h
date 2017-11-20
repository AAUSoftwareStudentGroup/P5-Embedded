#ifndef __MPU__
#define __MPU__

#include <Wire.h>
#include "config.h"
#include "datastructure.h"

int lastRecievedPacketTime = 0;
int mpuInterruptPin = 2;

void _mpu_setRegister(uint8_t addr, uint8_t val);

void mpu_setup();
datapoint mpu_read();

#endif