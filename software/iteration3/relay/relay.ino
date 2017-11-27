#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <stdlib.h>
#include "datastructures.h"
#include "config.h"
#include "ann.h"
#include "wifi.h"

static int LED_state = 0;
WiFiClient client;
network neuralNet;

void setup() {
  // setup serial for debugging
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);

  // Create access point
  Serial.println(String("Creating access point \"")+ WIFI_AP_SSID + "\" without password");
  WiFi.mode(WIFI_AP_STA);
  while(!WiFi.softAP(WIFI_AP_SSID)) {
    Serial.println("Failed to create access point");
  }

  // Connect to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print(String("Connecting to wifi: \"") + WIFI_SSID + "\" with pass: \"" + WIFI_PASS + "\"");
  // Wait for connection
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    digitalWrite(PIN_LED, LED_state++ % 2);
  }
  Serial.println("");

  // create ANN
  Serial.println("- ANN");
  setup_neuralNetwork();
  neuralNet = initiateRandomNetwork();
  
  // Start udp socket
  while(UDP.begin(8085) == 0);

  randomSeed(micros());
}

char* getMacFromNodeInput(char** buffer) {
  char* mac = *buffer;
  char* endOfMac = *buffer;
  
  while((*endOfMac) != '#') endOfMac++;

  *endOfMac = '\0';

  *buffer = endOfMac+1;

  return mac;
}

group getShotsFromNodeInput(char* buffer) {
  group shot;
  shot.length = 10;
  shot.datapoints = (datapoint*)malloc(sizeof(datapoint)*shot.length);;

  double parsedDatapoints[40];
      
  // For all datapoints
  for(int i = 0; i < 40; i++) {
    char* endOfNumber = buffer;
    // Find end of number
    while(*endOfNumber != ':') endOfNumber++;
    *endOfNumber = '\0';

    // Parse number
    parsedDatapoints[i] = String(buffer).toFloat();

    buffer = endOfNumber+1;
  }
  // Format array as group
  for(int i = 0; i < 10; i++) {
    shot.datapoints[i].X = parsedDatapoints[i*4];
    shot.datapoints[i].Y = parsedDatapoints[i*4+1];
    shot.datapoints[i].Z = parsedDatapoints[i*4+2];
    shot.datapoints[i].RX = parsedDatapoints[i*4+3];
  }
  return shot;
}

void relayResult(char* macAddr, networkResult annOut, network* ann) {
  static int nextSendTime = 0;

  String s = String(macAddr) + "#";
  for(int i = 0; i < annOut.length; i++) {
      s += String(ann->labels[i]) + ":" + String(annOut.results[i], 8) + ";";
  }

  if(nextSendTime - (int)millis() < 0 && WiFi.isConnected()) {
    Serial.println(s);
    nextSendTime = millis()+RELAY_BUFFERING_TIME_MS;

    // Connect to server
    if (!client.connect(HTTP_HOST, HTTP_PORT)) {
      Serial.println("Connection failed");
      return;
    }
   
    // Send buffer to server
    client.print(String("POST") + " /api/sensor/data HTTP/1.1\r\n" \
                 "Host: " + HTTP_HOST + "\r\n" \
                 "Content-Type: application/json\r\n" \
                 "Content-Length: "+ (s.length()+2) +"\r\n" \
                 "Connection: close\r\n\r\n"+"\"" + s + "\"");
    
    // Wait till done 
    int timeout = millis()+RELAY_HTTP_REQUEST_TIMEOUT_MS;
    while(client.connected()) {
      // If the request takes too long
      if (timeout-(int)millis() < 0) {
        Serial.println("Client Timeout !");
        break;
      }
      if(client.available()){
        String line = client.readStringUntil('\n');
        #ifdef DEBUG
          Serial.println(line);
        #endif
      }
    }
    client.stop();
  }
}

void loop() {
  static char* nodebuffer = (char*)malloc(sizeof(char)*NODE_BUFFER_SIZE);
  char* macAddrStr;

  // if theres data
  if(UDP.parsePacket()) {
    // data format: mac#1.23:4.56:1.23:4.56;1.23:4.56:1.23:4.56; * 12
    Serial.println("Dingding");
    int len = UDP.read(nodebuffer, NODE_BUFFER_SIZE);
    nodebuffer[len] = '\0'; 
    
    // save mac for writing output later
    Serial.println("nodebuffer: " + String(nodebuffer));
    macAddrStr = getMacFromNodeInput(&nodebuffer);
    
    // parse data as input for ann
    Serial.println("Mac: " + String(macAddrStr));
    group shot = getShotsFromNodeInput((char*)nodebuffer);
    
    #ifdef SERIALPRINTSHOT
      Serial.println("Length: " + String(shot.length));
      for(int i = 0; i < shot.length; i++) {
        Serial.println(String(i) + ".X : " + String(shot.datapoints[i].X));
        Serial.println(String(i) + ".Y : " + String(shot.datapoints[i].Y));
        Serial.println(String(i) + ".Z : " + String(shot.datapoints[i].Z));
        Serial.println(String(i) + ".RX: " + String(shot.datapoints[i].RX));
      }
    #endif

    // evaluate network
    Serial.println("Evaluating network");
    networkResult annOut = EvaluateNetwork(&neuralNet, shot);

    // Free shot array
    free(shot.datapoints);

    // send network result with mac prepended
    Serial.println("Sending result");
    relayResult(macAddrStr, annOut, &neuralNet);
  }
}