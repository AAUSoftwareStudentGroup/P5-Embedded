#include "I2Cdev.h"
#include <SPI.h>
#include <Wire.h>

#include "Adafruit_SSD1306.h"

#define MPU_ADDR 0x68

#define OLED_MOSI  4
#define OLED_CLK   5
#define OLED_DC    0
#define OLED_CS    2
#define OLED_RESET 16
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int mil;
int lastMillis;
int LED = 16;

int i = 0;

void setup_mpu6050() {
  Wire.begin(14,12);
  mpu6050_setRegister(0x6b, 0);
  mpu6050_setRegister(0x1b, 0b11000);
  mpu6050_setRegister(0x1c, 0b11000);
}

void mpu6050_setRegister(uint8_t addr, uint8_t val) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(addr);  // PWR_MGMT_1 register
  Wire.write(val);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);  
}

void mpu6050_get_data(
  int16_t* ax, int16_t* ay, int16_t* az, 
  int16_t* rx, int16_t* ry, int16_t* rz) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU_ADDR,(size_t)14,true);  // request a total of 14 registers
  *ax=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  *ay=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  *az=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Wire.read(); Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  *rx=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  *ry=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  *rz=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC); // No idea what it does, does not work without
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.setCursor(0,0);

  display.print("MPU ");
  display.display();

  pinMode(LED, OUTPUT);
  setup_mpu6050();

  display.println(" OK!");
  display.display();
  lastMillis = millis();
}

void loop() { 
  display.clearDisplay();
  display.setCursor(0,0);
  mil = millis();
  lastMillis = mil;
  
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU_ADDR,(size_t)14,true);  // request a total of 14 registers
  
  display.print("X : ");
  display.println((signed short)((signed short)Wire.read()<<8 | Wire.read()));  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  display.print("Y : ");
  display.println((signed short)((signed short)Wire.read()<<8 | Wire.read()));  // 0x3B (ACCEL_XOUT_H) | 0x3C (ACCEL_XOUT_L)   
  display.print("Z : ");
  display.println((signed short)((signed short)Wire.read()<<8 | Wire.read()));  // 0x3B (ACCEL_XOUT_H) | 0x3C (ACCEL_XOUT_L)   
  Wire.read(); Wire.read();  // 0x41 (TEMP_OUT_H) | 0x42 (TEMP_OUT_L)
  display.print("RX: ");
  display.println((signed short)((signed short)Wire.read()<<8 | Wire.read()));  // 0x3B (ACCEL_XOUT_H) | 0x3C (ACCEL_XOUT_L)   
  display.print("RY: ");
  display.println((signed short)((signed short)Wire.read()<<8 | Wire.read()));  // 0x3B (ACCEL_XOUT_H) | 0x3C (ACCEL_XOUT_L)   
  display.print("RZ: ");
  display.println((signed short)((signed short)Wire.read()<<8 | Wire.read()));  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   

  display.println(i++);
  display.display();
}
