#include <I2Cdev.h>

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <stdlib.h>
// #include <SPI.h>

// #include "Adafruit_SSD1306.h"
#include "datastructures.h"
#include "mpu.h"
#include "ann.h"

WiFiUDP client;
const char* ssid = "Mote_01";
// const char* password = "geronimo";
int state = 0;

int mil;
int lastMillis;
IPAddress recieverIP;

int LED = 10;
// int LED = 16;

#define OLED_MOSI  4
#define OLED_CLK   5
#define OLED_DC    0
#define OLED_CS    2
#define OLED_RESET 16
// Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// #if (SSD1306_LCDHEIGHT != 64)
// #error("Height incorrect, please fix Adafruit_SSD1306.h!");
// #endif

datapoint downSampleBuffer[20];
datapoint shotBuffer[10];

void setup_io() {
  // pinMode(LED, OUTPUT);

  // initialize serial communication for debugging purposes
  Serial.begin(115200);
}

void setup_lcd() {
  // display.begin(SSD1306_SWITCHCAPVCC); // No idea what it does, does not work without
  // display.setTextSize(1);
  // display.setTextColor(WHITE);

  // display.clearDisplay();
  // display.setCursor(0,0);
  // display.println("Initialized --- ");
  // display.println("testhest");
  // display.display();

}

void setup_wifi() {
  IPAddress defaultIP = client.remoteIP();

// Connect to wifi network
// WiFi.begin(ssid, password);
  while(!WiFi.softAP(ssid)) {
    Serial.println("Failed to create AP");
  }

//   Serial.print("Connecting to wifi");
//   // Wait for connection
//   while(WiFi.status() != WL_CONNECTED) {
//     Serial.print(".");
//     delay(500);
//     // digitalWrite(LED, state++ % 2);
//   }

  Serial.println();
  Serial.println("Waiting for app to say hi");

  // Print the IP address to serial
  // Serial.println(WiFi.localIP());

  client.begin(8085);
  // client.beginPacket(client.remoteIP(), 8085);
  while(client.remoteIP() == defaultIP) {
    delay(250);
    Serial.print(".");
    // digitalWrite(LED, state++ % 2);
  }
  recieverIP = client.remoteIP();
}


void setup() {

  setup_io();
  Serial.println("a");
  // setup_lcd();
  Serial.println("b");
  setup_neuralNetwork();
  Serial.println("c");
  reset_result();
  Serial.println("d");
  setup_wifi();
  Serial.println("e");
  setup_mpu6050(mpu_data_ready);
  Serial.println("f");
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
    setup_mpu6050(mpu_data_ready);
    return;
  }
  parseSample(p);
}

void sendString(String str) {
  client.beginPacket(recieverIP, 8085);
  client.write(str.c_str(), str.length());
  client.endPacket();
}

void handleIncomming() {
  // packet types:
  // - '.' keep alive
  // - 'R' Reset
  int packetSize = client.parsePacket();
  if (packetSize)
  {
    char packet[64];
    int len = client.read(packet, 64);
    if (len > 0)
    {
      packet[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", packet);
  }
}

void loop() {
  handleIncomming();
  // Serial.println("g");
  mpu_data_ready();
  // display.clearDisplay();
  // display.display();
  if(newResultReady) {
    newResultReady = false;


    for(int i = 0; i < currentResult.resultLength; i++) {
      String s = String();
      s += "P" + String(i+1) + ": " + String(currentResult.results[i]/n_results);
      // display.print("P"); display.print(i+1); display.print(": ");
      // display.println(currentResult.results[i]/n_results);
      sendString(s);
    }
  }
}
