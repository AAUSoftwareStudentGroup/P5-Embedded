#include "mpu.h"

void mpu6050_setRegister(uint8_t addr, uint8_t val) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(addr);  // PWR_MGMT_1 register
  Wire.write(val);    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);  
}

void setup_mpu6050(void (*interruptRutine)(void)) {
  // pinMode(mpuInterruptPin, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(mpuInterruptPin), interruptRutine, RISING);
  
  Wire.begin(14,12);
  // Wire.begin(5,4);
  mpu6050_setRegister(0x6b, 0);
  mpu6050_setRegister(0x1b, 0b11000);
  mpu6050_setRegister(0x1c, 0b11000);
  // mpu6050_setRegister(0x37, 0b10000); // clear interrupt on any read
  // mpu6050_setRegister(0x38, 0b1); // Enable data ready interrupt

  // Wire.setClock(3400000);
  Wire.setClock(400000);
}