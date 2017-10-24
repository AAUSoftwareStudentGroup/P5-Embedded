#include <I2Cdev.h>
// #include "MPU6050.h"
#include <Wire.h>

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

#define MPU_ADDR 0x68

WiFiUDP client;
const char* ssid = "Tardis";
const char* password = "geronimo";
bool state = true;

int b = 1; // Number of datapoints to read before sending. Change size of array on next line accordingly
int16_t data[100*7];
int sent;
int16_t dataindex;
int mil;
int lastMillis;
IPAddress ipaddr;

void setup_mpu6050() {
  Wire.begin(5,4);
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
int lastread;

void setup() {
    // initialize serial communication for debugging purposes
    Serial.begin(115200);

    Serial.print("Joining I2C\n");
    // join I2C bus
    setup_mpu6050();

    // Connect to wifi network
    WiFi.begin(ssid, password);
    
    Serial.print("Connecting to wifi");
    // Wait for connection
    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    
    Serial.print("\n");

    // Print the IP address to serial
    Serial.println(WiFi.localIP());

    lastMillis = millis();
    dataindex = 0;
    sent = 0;
    //Connect(); // No need to connect when using UDP
    client.begin(8085);
    client.beginPacket(client.remoteIP(), 8085);
    lastread = 0;
}

void loop() {
  client.beginPacket(client.remoteIP(), 8085);
  mil = millis();
  client.write((int8_t)(mil - lastMillis));
  lastMillis = mil;
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU_ADDR,(size_t)14,true);  // request a total of 14 registers
  client.write(Wire.read()); client.write(Wire.read());  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  client.write(Wire.read()); client.write(Wire.read());  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  client.write(Wire.read()); client.write(Wire.read());  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  Wire.read(); Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  client.write(Wire.read()); client.write(Wire.read());  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  client.write(Wire.read()); client.write(Wire.read());  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  client.write(Wire.read()); client.write(Wire.read());  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  client.endPacket();
}
