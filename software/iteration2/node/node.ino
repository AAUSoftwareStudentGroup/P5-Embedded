#include <I2Cdev.h>
#include <stdlib.h>

#include "config.h"
#include "datastructures.h"
#include "mpu.h"
#include "ann.h"
#include "wifi.h"
#include "test.h"

group networkFodder;
bool networkFodderReady = false;

void setup() {
  #ifdef TEST
    test();
  #endif
  
  setup_io();
  Serial.println();
  Serial.println("Initializing:");
  
  Serial.println("-test ANN");
  setup_neuralNetwork();

  Serial.println("- MPU");
  mpu_setup();

  Serial.println("- WIFI");
  setup_wifi();

  // setup a timer interrupt that triggers every 2ms
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer0InterruptHandler);
  // 2ms timeout
  timer0_write(ESP.getCycleCount()+TIMER0_INTERRUPT_TIME_MICRO_SECONDS*160);
  interrupts();

  Serial.println("Starting!");
}

void setup_io() {
  pinMode(PIN_LED, OUTPUT);

  // initialize serial communication for debugging purposes
  Serial.begin(115200);
}

void loop() {
  if(networkFodderReady) {
    networkFodderReady = false;
    networkResult result = EvaluateNetwork(networkFodder);

    // Send result over UDP
    // format - MACAddress#Label_1:Result1;Label_2:Result_2;...Label_N:Result_N;
    // example  9C:B6:D0:DE:CC:59#Anton:0.7932416;Morten:0.2178694;
    String s = String();
    s += String(macstr) + "#";
    for(int i = 0; i < result.resultLength; i++) {
        s += String(ann.labels[i]) + ":" + String(result.results[i], 8) + ";";
    }

    wifi_write(s);
    #ifdef DEBUG
    Serial.println(s);
    #endif
  }
}

void timer0InterruptHandler() {
  // Schedule next interrupt
  timer0_write(ESP.getCycleCount()+TIMER0_INTERRUPT_TIME_MICRO_SECONDS*160);

  // Get new sensor readings
  datapoint sensorData = mpu_read();

  // Process the data
  parseSample(sensorData);
}

void parseScaledSample(datapoint p) {
  static datapoint shotBuffer[10];
  static int shotBufferIndex = 0;

  // If a shot is already started OR this datapoint has a large enough rotation value to indicate that this is a new shot
  if(shotBufferIndex > 0 || (p.RX > 0.5 || p.RX < -0.5)) {
    shotBuffer[shotBufferIndex++] = p;
    if(shotBufferIndex >= 10) {
      shotBufferIndex = 0;
      
      networkFodder.length = 10;
      networkFodder.datapoints = shotBuffer;

      networkFodderReady = true;
    }
  }
}

void parseSample(datapoint p) {
  static datapoint downSampleBuffer[20];
  static int downSampleBufferIndex = 0;

  downSampleBuffer[downSampleBufferIndex++] = p;
  if(downSampleBufferIndex >= 20) {
    downSampleBufferIndex = 0;
    p = downSampleBuffer[0];
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
