#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <stdlib.h>
#include "datastructures.h"
#include "config.h"
#include "ann.h"

static int LED_state = 0;
WiFiClient client;
WiFiUDP UDP;
network neuralNet;

typedef struct _idToLabel {
  String nodeId;
  String label;
} idToLabel;

idToLabel nodeMapping[MAX_NUMBER_OF_NODES];
char* labels[NUMBER_OF_LABELS] = {"Anton", "Morten"};
float networkExpectedResultForLabel[NUMBER_OF_LABELS][NUMBER_OF_LABELS] = {{1,0}, {0,1}};

String outbuffer = String("");

//Traning/validation buffers
example buffers[NUMBER_OF_LABELS][MAX_DATA_BUFFER_SIZE]; // buffers are structured as: TRAIN_DATA_BUFFER_SIZE examples + VALIDATION_DATA_BUFFER_SIZE examples + additional new examples 
example* nextBufferSlot[NUMBER_OF_LABELS];

void setup() {
  // setup next buffer slots
  for(int i = 0; i < NUMBER_OF_LABELS; i++) {
    nextBufferSlot[i] = buffers[i]; //point to first element in buffer for each label i.  
  }
  
  // setup serial for debugging
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);

  Serial.println("- WIFI");
  setupWifi();

  Serial.println("- ANN");
  neuralNet = initiateRandomNetwork(labels, NUMBER_OF_LABELS);
  
  Serial.println("- UDP socket");
  while(UDP.begin(UDP_PORT) == 0);

  for(int i = 0; i < MAX_NUMBER_OF_NODES; i++) {
    nodeMapping[i].nodeId = String("");
    nodeMapping[i].label = String("");
  }

  randomSeed(micros());
  Serial.println("SETUP: DONE!");
}

void setupWifi() {
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
}

void parseMac(char* buffer, char** mac, char** endOfMac) {
  *mac = buffer;
  
  while(buffer[0] != '#') 
    buffer++;
  buffer[0] = '\0';

  *endOfMac = buffer+1;
}




group parseShot(char* buffer) {
  group shot;
  shot.length = 10;
  shot.datapoints = (datapoint*)malloc(sizeof(datapoint)*shot.length);;

  float parsedDatapoints[40];
  
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

// returns a boolean, indicating if there is a label for the given node id
bool getLabelForNode(char* macCString, networkResult* result, int* labelIndex) {
  String mac = String(macCString);
  
  bool foundID = false;
  bool foundLabel = false;
  // find the mac in the mapping array
  for(int i = 0; i < MAX_NUMBER_OF_NODES; i++) {
    if(nodeMapping[i].nodeId == mac) {
      foundID = true;
      // find the appropriate label
      for(int j = 0; j < NUMBER_OF_LABELS; j++) {
        if(nodeMapping[i].label == String(labels[j])) {
          foundLabel = true;
          result->results = networkExpectedResultForLabel[j];
          result->length = NUMBER_OF_LABELS;
          *labelIndex = j;
        }
      }
    }
  }
  if(!foundID) {
    // add id to mapping array
    for(int i = 0; i < MAX_NUMBER_OF_NODES; i++) {
      if(nodeMapping[i].nodeId == "") {
        nodeMapping[i].nodeId = mac;
        nodeMapping[i].label = "";
        break;
      }
    }
  }
  return foundLabel;
}

bool thereIsNewNodeData() {
  return (UDP.parsePacket() > 0);
}

float calculateMSE(network *n) {
  float error = 0;
  int count = 0;
  for(int i = 0; i < NUMBER_OF_LABELS; i++) {
    for(int d = 0; d < VALIDATION_DATA_BUFFER_SIZE; d++) {
      example e = buffers[i][d + TRAIN_DATA_BUFFER_SIZE];
      networkResult actualOutput = EvaluateNetwork(n, e.input);
      networkResult expectedOutput = e.output;
      for(int r = 0; r < actualOutput.length; r++) {
        error += (actualOutput.results[r] - expectedOutput.results[r]) * (actualOutput.results[r] - expectedOutput.results[r]); 
        count++;
      }
    }
  }
  return error / count;
}

void train(network *n) {
  for(int i = 0; i < MAX_EPOCHS; i++) {
    float validationError = calculateMSE(n);
    
    Serial.println(String("Validation error: ") + validationError);

    if(validationError < VALIDATION_ERROR_THRESHOLD) {
      break;
    }

    example trainingData[TRAIN_DATA_BUFFER_SIZE * NUMBER_OF_LABELS];

    for(int d = 0; d < TRAIN_DATA_BUFFER_SIZE; d++) {
      for(int i = 0; i < NUMBER_OF_LABELS; i++) {
        trainingData[d * NUMBER_OF_LABELS + i] = buffers[i][d];    
      }
    }
    
    trainNetwork(n, trainingData, TRAIN_DATA_BUFFER_SIZE * NUMBER_OF_LABELS);
  }
}

void onNewNodeData() {
  static char* udpInputBuffer = (char*)malloc(sizeof(char)*NODE_BUFFER_SIZE);
  char* macAddrStr;
  char* shotStr;
  group shot;
  networkResult annOut;
  
  // read data
  int len = UDP.read(udpInputBuffer, NODE_BUFFER_SIZE-1);
  udpInputBuffer[len] = '\0'; 
  
  // read macaddress and shot data
  parseMac(udpInputBuffer, &macAddrStr, &shotStr);
  shot = parseShot(shotStr);

  example e;
  e.input = shot;
  int labelIndex;
  if(getLabelForNode(macAddrStr, &(e.output), &labelIndex)) {
    Serial.println(nextBufferSlot[labelIndex] - buffers[labelIndex]);
    if(nextBufferSlot[labelIndex] - buffers[labelIndex] < MAX_DATA_BUFFER_SIZE) { // If buffer for label is not full
      *(nextBufferSlot[labelIndex]) = e; // Add example to buffer for label with index i;
      nextBufferSlot[labelIndex] += 1; // Move next available slot
    } else {
      free(shot.datapoints);
    }
  } else {
    annOut = EvaluateNetwork(&neuralNet, shot);
    outbuffer += String(macAddrStr) + "#";
    for(int i = 0; i < annOut.length; i++) {
      outbuffer += String(neuralNet.labels[i]) + ":" + String(annOut.results[i], 8) + ";";
    }
    outbuffer += String("\n");
    free(shot.datapoints);
  }

  
}

bool itsTimeToSendToServer() {
  static int nextSendTime = 0;
  
  if(nextSendTime - (int)millis() < 0 && WiFi.isConnected()) {
    nextSendTime = millis()+RELAY_BUFFERING_TIME_MS;
    return true;
  }
  return false;
}

void setMapping(char* id, char* label) {
  for(int i = 0; i < MAX_NUMBER_OF_NODES; i++) {
    if(nodeMapping[i].nodeId == id) {
      nodeMapping[i].label = label;
    }
  }
}

void parseServerResponse(char* serverResopnse) {
  // server response format
  // "345645678910#Label1;A020A61AD519#;2C3AE8340553#invalidString;"

  char* id;
  char* label;
  if(*serverResopnse == '"') {
    serverResopnse++;
  }

  id = serverResopnse;
  while(*serverResopnse != '"') {
    // if reached end of id
    if(*serverResopnse == '#') {
      *serverResopnse = '\0';
      // begin reading label
      label = serverResopnse+1;
    }
    // if reached end of label
    if(*serverResopnse == ';') {
      *serverResopnse = '\0';

      // update mapping array
      setMapping(id, label);

      // begin reading label
      id = serverResopnse+1;

    }
    serverResopnse++;
  }
}

void sendDataToServer() {
  // Connect to server
  if (!client.connect(HTTP_HOST, HTTP_PORT)) {
    Serial.println("Connection failed");
    return;
  }
 
  Serial.println(outbuffer);
  // Send buffer to server
  client.print(String("POST") + " /api/sensor/data HTTP/1.1\r\n" \
               "Host: " + HTTP_HOST + "\r\n" \
               "Content-Type: application/json\r\n" \
               "Content-Length: "+ (outbuffer.length()+2) +"\r\n" \
               "Connection: close\r\n\r\n"+"\"" + outbuffer + "\"");
  
  // Wait till done 
  int timeout = millis()+RELAY_HTTP_REQUEST_TIMEOUT_MS;
  bool currentlyReadingHeader = true;
  while(client.connected()) {
    // If the request takes too long
    if (timeout-(int)millis() < 0) {
      Serial.println("Client Timeout !");
      break;
    }
    if(client.available()){
      // read result and update node-label mapping
      String line = client.readStringUntil('\n');
      if(currentlyReadingHeader == false) {
        Serial.print("[DATAHUB] ");
        Serial.print(line);
        Serial.println(" [/DATAHUB]");
        char* serverResopnse = (char*)malloc(sizeof(char)*line.length());
        strcpy(serverResopnse, line.c_str());
        parseServerResponse(serverResopnse);
        free(serverResopnse);
      }
      if(line.length() < 2) {
        currentlyReadingHeader = false;
      }
    }
  }
  client.stop();
  outbuffer = String("");
}

bool trainplz = true;

void loop() {
  static int nConnectedNodes = 0;

  if(thereIsNewNodeData()) {
    onNewNodeData();
  }
  bool trainingAvailable = true;
  for(int i = 0; i < NUMBER_OF_LABELS; i++) {
    if(nextBufferSlot[i] - buffers[i] < TRAIN_DATA_BUFFER_SIZE + VALIDATION_DATA_BUFFER_SIZE) {
      trainingAvailable = false;
      break;
    }
  }
  
  if(trainingAvailable && trainplz) {
    train(&neuralNet);
    trainplz = false;
  }
  if(itsTimeToSendToServer()) {
    if(WiFi.softAPgetStationNum() != nConnectedNodes) {
      nConnectedNodes = WiFi.softAPgetStationNum();
      Serial.println(String("Connected devices: " + String(nConnectedNodes)));
    }

    Serial.println("-MAPPING-");
    for(int i = 0; i < MAX_NUMBER_OF_NODES; i++) {
      Serial.print(nodeMapping[i].nodeId);
      Serial.print(" - ");
      Serial.println(nodeMapping[i].label);
    }

    sendDataToServer();
  }
}
