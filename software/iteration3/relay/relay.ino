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
double networkExpectedResultForLabel[NUMBER_OF_LABELS][NUMBER_OF_LABELS] = {{1,0}, {0,1}};

String outbuffer = String("");

void setup() {
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

// returns a boolean, indicating if there is a label for the given node id
bool getLabelForNode(char* macCString, networkResult* result) {
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
          result->length = 2;
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

  // evaluate network and add to out buffer
  annOut = EvaluateNetwork(&neuralNet, shot);
  outbuffer += String(macAddrStr) + "#";
  for(int i = 0; i < annOut.length; i++) {
    outbuffer += String(neuralNet.labels[i]) + ":" + String(annOut.results[i], 8) + ";";
  }
  outbuffer += String("\n");


  // If node has been maped to a label
  example e;
  e.input = shot;
  if(getLabelForNode(macAddrStr, &(e.output))) {
    Serial.println(String(macAddrStr) + " training with data " + String(e.output.results[0])+","+String(e.output.results[1]));
    trainNetwork(&neuralNet, &e, 1);
  }

  // Free shot array
  free(shot.datapoints);
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
      }
      if(line.length() < 2) {
        currentlyReadingHeader = false;
      }
    }
  }
  client.stop();
  outbuffer = String("");
}

void loop() {
  static int nConnectedNodes = 0;

  if(thereIsNewNodeData()) {
    onNewNodeData();
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