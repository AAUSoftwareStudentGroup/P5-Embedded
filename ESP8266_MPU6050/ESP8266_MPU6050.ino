#include <I2Cdev.h>
// #include "MPU6050.h"
#include <Wire.h>

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

#define MPU_ADDR 0x68

WiFiUDP client;
const char* ssid = "Tardis";
const char* password = "geronimo";
int state = 0;
int stillConnected = 1;

int mil;
int lastMillis;
IPAddress recieverIP;

int lastread;
int LED = 16;

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

void setup() {
    pinMode(LED, OUTPUT);
    IPAddress defaultIP = client.remoteIP();

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
        digitalWrite(LED, state++ % 2);
    }
    
    Serial.print("\n");

    // Print the IP address to serial
    Serial.println(WiFi.localIP());

    lastMillis = millis();
    //Connect(); // No need to connect when using UDP
    client.begin(8085);
    // client.beginPacket(client.remoteIP(), 8085);
    while(client.remoteIP() == defaultIP) {
      delay(250);
      digitalWrite(LED, state++ % 2);
    }
    recieverIP = client.remoteIP();
    lastread = 0;
    state = 0;
}

void loop() {
  // When receiving packet, reset stillConnected to like 100. If no packet was received in 100 datapoints, turn off LED
  if(client.parsePacket()) {
    stillConnected = 50;
  }
  client.beginPacket(recieverIP, 8085);
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
  digitalWrite(LED, stillConnected-- > 0);
}
