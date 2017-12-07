#include <WiFiUdp.h>
// #include <WiFi.h>
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
  char nodeId[MAC_SIZE];
  char* label;
} idToLabel;

idToLabel nodeMapping[MAX_NUMBER_OF_NODES];
char* labels[NUMBER_OF_LABELS] = {"Player1", "Player2"};
float networkExpectedResultForLabel[NUMBER_OF_LABELS][NUMBER_OF_LABELS] = {{1,0}, {0,1}};

String outbuffer = String("");

//Traning/validation buffers
example buffers[NUMBER_OF_LABELS][MAX_DATA_BUFFER_SIZE]; // buffers are structured as: TRAIN_DATA_BUFFER_SIZE examples + VALIDATION_DATA_BUFFER_SIZE examples + additional new examples 
example* nextBufferSlot[NUMBER_OF_LABELS];

void logline(const char *alias, const char *message) {
  Serial.print(millis(), DEC);
  Serial.print(":");
  Serial.print(alias);
  Serial.print(":");
  Serial.println(message);
}

void setup() {
  logline("setup", "started");
  // setup next buffer slots
  for(int i = 0; i < NUMBER_OF_LABELS; i++) {
    nextBufferSlot[i] = buffers[i]; //point to first element in buffer for each label i.  
  }
  
  for(int i = 0; i < MAX_NUMBER_OF_NODES; i++) {
    nodeMapping[i].nodeId[0] = '\0';
    nodeMapping[i].label = NONE_STR;
  }

  // setup serial for debugging
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);

  logline("setupWifi", "started");
  setupWifi();
  logline("setupWifi", "done");

  logline("initiateRandomNetwork", "started");
  logline("INPUT_NEURONS", String(INPUT_NEURONS).c_str());
  logline("HIDDEN_NEURONS", String(HIDDEN_NEURONS).c_str());
  logline("NUMBER_OF_LABELS", String(NUMBER_OF_LABELS).c_str());
  neuralNet = initiateRandomNetwork(labels);
  logline("initiateRandomNetwork", "done");

  logline("UDP.begin", "started");
  while(UDP.begin(UDP_PORT) == 0);
  logline("UDP.begin", "done");

  randomSeed(micros());
  logline("setup", "done");
}

void setupWifi() {

  // Create access point
  logline("setup access point", "started");
  logline("WIFI_AP_SSID", WIFI_AP_SSID);
  WiFi.mode(WIFI_AP_STA);
  while(!WiFi.softAP(WIFI_AP_SSID)) {
    logline("setup access point", "failed");
  }
  logline("setup access point", "done");
  // Connect to wifi
  logline("connect to access point", "started");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  logline("WIFI_SSID", WIFI_SSID);
  logline("WIFI_PASS", WIFI_PASS);
  // Wait for connection
  while(WiFi.status() != WL_CONNECTED) {
    logline("WiFi status", "not connected");
    delay(500);
    digitalWrite(PIN_LED, LED_state++ % 2);
  }
  logline("WiFi status", "connected");
  logline("connect to access point", "done");
}

void parseMac(char* buffer, char** mac, char** endOfMac) {
  *mac = buffer;
  
  while(buffer[0] != '#' && buffer-(*mac) <= MAC_SIZE)
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
    while(*endOfNumber != ':' && *endOfNumber != '\0' && endOfNumber-buffer < 20) endOfNumber++;
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
  bool foundID = false;
  bool foundLabel = false;
  // find the mac in the mapping array
  for(int i = 0; i < MAX_NUMBER_OF_NODES; i++) {
    if(strcmp(nodeMapping[i].nodeId, macCString) == 0) {
      foundID = true;
      // find the appropriate label
      for(int j = 0; j < NUMBER_OF_LABELS; j++) {
        if(strcmp(nodeMapping[i].label, labels[j]) == 0) {
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
      if(nodeMapping[i].nodeId[0] == '\0') {
        strcpy(nodeMapping[i].nodeId, macCString);
        nodeMapping[i].label = NONE_STR;
        break;
      }
    }
  }
  return foundLabel;
}

bool thereIsNewNodeData() {
  return (UDP.parsePacket() > 0);
}

//Calculate networks mean squared error on validation data in buffers
float calculateMSE(network *n) {
  float errorSum = 0;
  float error;
  int count = 0;
  for(int i = 0; i < NUMBER_OF_LABELS; i++) {
    for(int d = 0; d < VALIDATION_DATA_BUFFER_SIZE; d++) {
      example e = buffers[i][d + TRAIN_DATA_BUFFER_SIZE];
      networkResult actualOutput = EvaluateNetwork(n, e.input);
      networkResult expectedOutput = e.output;
      for(int r = 0; r < actualOutput.length; r++) {
        error = (actualOutput.results[r] - expectedOutput.results[r]);
        errorSum += error*error; 
        count++;
      }
    }
  }
  return errorSum / count;
}

//Shuffle data using Fisher-Yates shuffle algorithm
void shuffleData(example *data, int count) {
   for(int i = 0; i < count; i++) {
    int randomIndex = random(count);
    example temp = data[i];
    data[i] = data[randomIndex];
    data[randomIndex] = temp;
  }
}

void train(network *n) {
  static float lowerstValidationError = 100000000;
  static int numberOfDidNotImproveTrainings = 0;
  static bool copyBuffers = true;
  static example trainingData[TRAIN_DATA_BUFFER_SIZE * NUMBER_OF_LABELS];

  //Only copy buffers to trainingData the first iteration of each training session
  if(copyBuffers) {
    copyBuffers = false;
    for(int i = 0; i < NUMBER_OF_LABELS; i++) { //Shuffle data buffers to get validation data across randomly from data collected
      shuffleData(buffers[i], TRAIN_DATA_BUFFER_SIZE + VALIDATION_DATA_BUFFER_SIZE);
    }
    for(int d = 0; d < TRAIN_DATA_BUFFER_SIZE; d++) {
      for(int i = 0; i < NUMBER_OF_LABELS; i++) {
        trainingData[d * NUMBER_OF_LABELS + i] = buffers[i][d];    
      }
    }
  }

  //Shuffle traning data each iteration
  shuffleData(trainingData, TRAIN_DATA_BUFFER_SIZE * NUMBER_OF_LABELS);

  float validationError = calculateMSE(n);
  logline("validationError", String(validationError,4).c_str());

  if(validationError < lowerstValidationError-REQUIRED_IMPROVEMENT) {
    lowerstValidationError = validationError;
    numberOfDidNotImproveTrainings = 0;
  }
  else if(numberOfDidNotImproveTrainings++ > ALLOWED_ITERATIONS_WHERE_TRAINING_DOES_NOT_IMPROVE) {
    lowerstValidationError = 100000000;
    numberOfDidNotImproveTrainings = 0;
    // empty the buffer
    for(int l = 0; l < NUMBER_OF_LABELS; l++) {
      for(int d = 0; d < MAX_DATA_BUFFER_SIZE; d++) {
        free(buffers[l][d].input.datapoints);
      }
      nextBufferSlot[l] = buffers[l];
    }
    copyBuffers = true;
    logline("train", "done");
    return;
  }
  trainNetwork(n, trainingData, TRAIN_DATA_BUFFER_SIZE * NUMBER_OF_LABELS);
}

void onNewNodeData() {
  static char udpInputBuffer[NODE_BUFFER_SIZE];
  char* macAddrStr;
  char* shotStr;
  group shot;
  networkResult annOut;
  // read data
  int len = UDP.read(udpInputBuffer, NODE_BUFFER_SIZE-1);
  udpInputBuffer[len] = '\0'; 
  
  logline("udpInputBuffer", udpInputBuffer);
  
  // read macaddress and shot data
  parseMac(udpInputBuffer, &macAddrStr, &shotStr);
  shot = parseShot(shotStr);
  
  example e;
  e.input = shot;
  int labelIndex;
  if(getLabelForNode(macAddrStr, &(e.output), &labelIndex)) {
    logline(labels[labelIndex], String(nextBufferSlot[labelIndex] - buffers[labelIndex]).c_str());
    if(nextBufferSlot[labelIndex] - buffers[labelIndex] < MAX_DATA_BUFFER_SIZE) { // If buffer for label is not full
      *(nextBufferSlot[labelIndex]) = e; // Add example to buffer for label with index i;
      nextBufferSlot[labelIndex] += 1; // Move next available slot
    } else {
      free(shot.datapoints);
    }
  } else {
    if(outbuffer.length() < 1024) {
      annOut = EvaluateNetwork(&neuralNet, shot);
      outbuffer += String(macAddrStr) + "#";
      for(int i = 0; i < annOut.length; i++) {
        outbuffer += String(neuralNet.labels[i]) + ":" + String(annOut.results[i], 8) + ";";
      }
      outbuffer += String("\n");
    }
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
    if(strcmp(nodeMapping[i].nodeId, id) == 0) {
      nodeMapping[i].label = NONE_STR;
      for(int j = 0; j < NUMBER_OF_LABELS; j++) {
        if(strcmp(labels[j], label) == 0) {
          nodeMapping[i].label = labels[j];
        }
      }
      return;
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
      // Serial.println(id);
      // Serial.println(label);
      setMapping(id, label);

      // begin reading label
      id = serverResopnse+1;

    }
    serverResopnse++;
  }
}

void sendDataToServer() {
  static int timeout;
  static bool currentlyReadingHeader;

  if(client.connected()) {
    // If the request takes too long
    if (timeout-(int)millis() < 0) {
      logline("client", "timeout");
      client.stop();
      return;
    }
    if(client.available()){
      // read result and update node-label mapping
      String line = client.readStringUntil('\n');
      
      if(currentlyReadingHeader == false) {
      
        logline("datahub response", line.c_str());
        char* serverResopnse = (char*)malloc(sizeof(char)*(line.length()+1));
        
        strcpy(serverResopnse, line.c_str());
        parseServerResponse(serverResopnse);
        free(serverResopnse);
      }
      if(line.length() < 2) {
        currentlyReadingHeader = false;
      }
      
    }
  }
  else {
    client.stop();
    // Connect to server
    logline("connecting to server", "started");
    if (!client.connect(HTTP_HOST, HTTP_PORT)) {
      logline("connecting to server", "failed");
      return;
    }
    logline("connecting to server", "done");
    logline("outbuffer", outbuffer.c_str());
    // Send buffer to server
    logline("sending buffer", "started");
    client.print(String("POST") + " /api/sensor/data HTTP/1.1\r\n" \
                 "Host: " + HTTP_HOST + "\r\n" \
                 "Content-Type: application/json\r\n" \
                 "Content-Length: "+ (outbuffer.length()+2) +"\r\n" \
                 "Connection: close\r\n\r\n"+"\"" + outbuffer + "\"");
    logline("sending buffer", "done");
    outbuffer = String("");

    // Wait till done 
    timeout = millis()+RELAY_HTTP_REQUEST_TIMEOUT_MS;
    currentlyReadingHeader = true;
  }
}


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
  
  if(trainingAvailable){
    train(&neuralNet);
  }

  if(itsTimeToSendToServer() && !client.connected()) {
    logline("free heap", String(ESP.getFreeHeap()).c_str());
  
    if(WiFi.softAPgetStationNum() != nConnectedNodes) {
      nConnectedNodes = WiFi.softAPgetStationNum();
      logline("connected devices", String(nConnectedNodes).c_str());
    }
    
    for(int i = 0; i < MAX_NUMBER_OF_NODES; i++) {
      logline(nodeMapping[i].nodeId, nodeMapping[i].label);
    }
    
    sendDataToServer();
  }
  if(client.connected()) {
    sendDataToServer();
  }
}
