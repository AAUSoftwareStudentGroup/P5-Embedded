#include <I2Cdev.h>
#include <stdlib.h>

#include "config.h"
#include "datastructures.h"
#include "mpu.h"
#include "ann.h"
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
  
  Serial.println("- ANN ...");
  setup_neuralNetwork();
  resetResult();

  Serial.println("- MPU ...");
  mpu_setup();

  Serial.println("- WIFI ...");
  setup_wifi();

  Serial.println("Starting!");
}

int n_results;
bool newResultReady;
networkResult currentResult;

void loop() {
  static int mpuNextReadReady = 0;
  datapoint sensorData;

  if(mpuNextReadReady-micros() < 0) {
    mpuNextReadReady = micros()+MPU_READ_DIFFTIME_MICRO_SECONDS;
    sensorData = mpu_read();
    parseSample(sensorData);
  }

  if(newResultReady) {
    newResultReady = false;

    // for(int i = 0; i < currentResult.resultLength; i++) {
    //   String s = String();
    //   // current accumulated result is prepended uppercase P
    //   s += "P" + String(i+1) + ": " + String(currentResult.results[i]/n_results);
    //   write(s);
    // }
    String s = String();
    s += String(macstr) + "#";
    for(int i = 0; i < currentResult.resultLength; i++) {
      // current result is prepended lowercase p
      s += "p" + String(i+1) + ": " + String(currentResult.results[i]);
      write(s);
    }
  }
}


void write(String s) {
  wifi_write(s);

  #ifdef DEBUG
  Serial.println(s);
  #endif
}

void resetResult() {
  currentResult.resultLength = (ann.layers+ann.n_layers-1)->n_nodes;
  currentResult.results = (double*)malloc(sizeof(double)*currentResult.resultLength);
  for(int i = 0; i < currentResult.resultLength; i++)
    currentResult.results[i] = 0;
  n_results = 0;
  newResultReady = false;
}

void handleNewResult(networkResult result) {
  // for(int i = 0; i < currentResult.resultLength; i++)
  //   currentResult.results[i] += result.results[i];
  // n_results++;
  // newResultReady = true;

  for(int i = 0; i < currentResult.resultLength; i++)
    currentResult.results[i] = result.results[i];
  newResultReady = true;
}

void parseScaledSample(datapoint p) {
  static datapoint shotBuffer[10];
  static int shotBufferIndex = 0;

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
  static datapoint downSampleBuffer[20];
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
