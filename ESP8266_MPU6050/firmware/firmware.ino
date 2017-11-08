#include <I2Cdev.h>

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <stdlib.h>
#include <SPI.h>

#include "Adafruit_SSD1306.h"
#include "datastructures.h"
#include "mpu.h"
#include "ann.h"

WiFiUDP client;
const char* ssid = "Tardis";
const char* password = "geronimo";
int state = 0;

int mil;
int lastMillis;
IPAddress recieverIP;

int LED = 16;

#define OLED_MOSI  4
#define OLED_CLK   5
#define OLED_DC    0
#define OLED_CS    2
#define OLED_RESET 16
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

datapoint downSampleBuffer[20];
datapoint shotBuffer[10];

void setup_io() {
  pinMode(LED, OUTPUT);

  // initialize serial communication for debugging purposes
  Serial.begin(115200);
}

void setup_lcd() {
  display.begin(SSD1306_SWITCHCAPVCC); // No idea what it does, does not work without
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Initialized");
  display.display();
}

void setup_wifi() {
  IPAddress defaultIP = client.remoteIP();

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
  state = 0;
}


void setup() {
  setup_io();
  setup_lcd();
  setup_neuralNetwork();
  reset_result();
  // setup_wifi();
  setup_mpu6050(mpu_data_ready);
  Serial.print("Starting\n");
}

int n_results;
bool newResultReady;
networkResult currentResult;
void reset_result() {
  currentResult.resultLength = (ann.layers+ann.n_layers-1)->n_nodes;
  currentResult.results = (double*)malloc(sizeof(double)*currentResult.resultLength);
  for(int i = 0; i < currentResult.resultLength; i++)
    currentResult.results[i] = 0;
  n_results = 0;
  newResultReady = false;
}

void handleNewResult(networkResult result) {
  for(int i = 0; i < currentResult.resultLength; i++)
    currentResult.results[i] += result.results[i];
  n_results++;
  newResultReady = true;
}

void parseScaledSample(datapoint p) {
  static int shotBufferIndex = 0;
  // Serial.println(p.RX);
  if(shotBufferIndex > 0 || (p.RX > 0.5 || p.RX < -0.5)) {
    shotBuffer[shotBufferIndex++] = p;
    if(shotBufferIndex >= 10) {
      shotBufferIndex = 0;
      group g;
      g.length = 10;
      g.datapoints = shotBuffer;
      networkResult result = EvaluateNetwork(g);

      handleNewResult(result);
    }
  }
}

void parseSample(datapoint p) {
  static int downSampleBufferIndex = 0;
  downSampleBuffer[downSampleBufferIndex++] = p;
  if(downSampleBufferIndex >= 20) {
    downSampleBufferIndex = 0;
    datapoint p = downSampleBuffer[0];
    for(int i = 1; i < 20; i++) {
      p.X += downSampleBuffer[i].X;
      p.Y += downSampleBuffer[i].Y;
      p.Z += downSampleBuffer[i].Z;
      p.RX += downSampleBuffer[i].RX;
      p.RY += downSampleBuffer[i].RY;
      p.RZ += downSampleBuffer[i].RZ;
    }
    p.X   /= 40000.0; // 20*2000
    p.Y   /= 40000.0;
    p.Z   /= 40000.0;
    p.RX  /= 200000.0; // 20*10000
    p.RY  /= 200000.0;
    p.RZ  /= 200000.0;
    parseScaledSample(p);
  }
}

int nextSampleTime = 0;
void mpu_data_ready() {
  if(nextSampleTime-micros() < 0) {
    // Wire.beginTransmission(MPU_ADDR);
    // Wire.write(0x3A);  // starting with register 0x3B (ACCEL_XOUT_H)
    // Wire.endTransmission(false);
    // Wire.requestFrom((uint8_t)MPU_ADDR,(size_t)1,true);
    // Wire.read();
    return;
  }
  nextSampleTime+=2000;

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
  }
  parseSample(p);




  // client.beginPacket(recieverIP, 8085);
  // mil = millis();
  // client.write((int8_t)(mil - lastMillis));
  // lastMillis = mil;
  // Wire.beginTransmission(MPU_ADDR);
  // Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  // Wire.endTransmission(false);
  // Wire.requestFrom((uint8_t)MPU_ADDR,(size_t)14,true);  // request a total of 14 registers
  // client.write(Wire.read()); client.write(Wire.read());  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  // client.write(Wire.read()); client.write(Wire.read());  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  // client.write(Wire.read()); client.write(Wire.read());  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  // Wire.read(); Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  // client.write(Wire.read()); client.write(Wire.read());  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  // client.write(Wire.read()); client.write(Wire.read());  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  // client.write(Wire.read()); client.write(Wire.read());  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  // client.endPacket();
}

void loop() {
  mpu_data_ready();
  if(newResultReady) {
    newResultReady = false;
    
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("---Result---");
    for(int i = 0; i < currentResult.resultLength; i++) {
      display.print("P"); display.print(i+1); display.print(": ");
      display.println(currentResult.results[i]/n_results);
    }
    // display.println(output[0]);
    // display.println(output[1]);
    // display.println(tick);
    display.display();
  }
}
