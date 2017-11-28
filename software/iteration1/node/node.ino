#include <I2Cdev.h>
#include <stdlib.h>

#include "config.h"
#include "datastructures.h"
#include "mpu.h"
#include "wifi.h"


void setup_io() {
  pinMode(PIN_LED, OUTPUT);

  // initialize serial communication for debugging purposes
  Serial.begin(115200);
}

void setup() {

  setup_io();
  Serial.println();
  Serial.println("Setup:");
  
  Serial.print("- MPU ...");
  mpu_setup();
  Serial.println(" OK");

  Serial.print("- WIFI ...");
  setup_wifi();
  Serial.println(" OK");
  Serial.print("IP address is ");
  Serial.println(wifi_localIP());

  Serial.println("Starting!");
}

void loop() {
  wifi_read(); // To parse incomming packet and update remoteIP
  if(wifi_remoteIP() != IPAddress(0, 0, 0, 0)) {
    Serial.println(wifi_remoteIP());
    wifi_setReceiverIP(wifi_remoteIP());
  }

  static int mpuNextReadReady = 0;
  datapoint sensorData;

  if(mpuNextReadReady - (int)micros() < 0) {
    mpuNextReadReady = micros() + MPU_READ_DIFFTIME_MICRO_SECONDS;
    sensorData = mpu_read();
    wifi_write_datapoint(sensorData);
  }
}

void wifi_write_datapoint(datapoint dp) {
  String s = "";
  s += String(dp.T/(2<<15), 2); s += ";";
  s += String(dp.X/(2<<15), 2); s += ";";
  s += String(dp.Y/(2<<15), 2); s += ";";
  s += String(dp.Z/(2<<15), 2); s += ";";
  s += String(dp.RX/(2<<15), 2); s += ";";
  s += String(dp.RY/(2<<15), 2); s += ";";
  s += String(dp.RZ/(2<<15), 2); s += ";";

  wifi_write(s);
}

void write(String s) {
  wifi_write(s);

  #ifdef DEBUG
  Serial.println(s);
  #endif
}