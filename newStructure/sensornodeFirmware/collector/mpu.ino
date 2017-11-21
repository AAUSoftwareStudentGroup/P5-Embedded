#include "mpu.h"

void _mpu_setRegister(uint8_t addr, uint8_t val) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(addr);
  Wire.write(val);
  Wire.endTransmission(true);  
}

void mpu_setup() {
  Wire.begin(PIN_MPU_SDA, PIN_MPU_SCL);
  _mpu_setRegister(0x6b, 0);
  _mpu_setRegister(0x1b, 0b11000);
  _mpu_setRegister(0x1c, 0b11000);

  Wire.setClock(400000);
}

datapoint mpu_read() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU_ADDR,(size_t)14,true);  // request a total of 14 registers
  short tmp;
  datapoint p;
  // Serial.println(Wire.read());
  tmp = Wire.read() << 8 | Wire.read(); p.X = double(tmp);
  tmp = Wire.read() << 8 | Wire.read(); p.Y = double(tmp);
  tmp = Wire.read() << 8 | Wire.read(); p.Z = double(tmp);
  Wire.read(); Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  tmp = Wire.read() << 8 | Wire.read(); p.RX = double(tmp);
  tmp = Wire.read() << 8 | Wire.read(); p.RY = double(tmp);
  tmp = Wire.read() << 8 | Wire.read(); p.RZ = double(tmp);
  if(p.X == p.Y && p.X == p.Z && p.X == p.RX) {
    Serial.println("SENSOR ERROR");
    mpu_setup();
    return p;
  }
  return p;
}