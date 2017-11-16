#ifndef __MPU__
#define __MPU__

#include <Wire.h>

#define MPU_ADDR 0x68
int lastRecievedPacketTime = 0;
int mpuInterruptPin = 2;

void mpu6050_setRegister(uint8_t addr, uint8_t val);
void setup_mpu6050(void (*interruptRutine)(void));

#endif