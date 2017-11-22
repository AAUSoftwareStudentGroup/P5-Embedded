#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <stdlib.h>

#define WIFI_AP_SSID "FootballRelay"
#define WIFI_SSID "wifi"
#define WIFI_PASS "TannerHelland"
#define UDP_PORT 8085
#define HTTP_HOST "p5datahub.azurewebsites.net"
#define HTTP_PORT 80
#define RELAY_BUFFERING_TIME_MS 3000
#define RELAY_HTTP_REQUEST_TIMEOUT_MS 2000

#define PIN_LED 16
#define NODE_BUFFER_SIZE 128
#define NODE_BUFFER_LENGTH 128

static int LED_state = 0;
char nodebuffer[NODE_BUFFER_SIZE];
String s;
WiFiUDP UDP;
WiFiClient client;

void setup() {
  // setup serial for debugging
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);

  // create AP
  Serial.println();
  Serial.println();

  Serial.println(String("Creating access point \"")+ WIFI_AP_SSID + "\" without password");
  WiFi.mode(WIFI_AP_STA);
  while(!WiFi.softAP(WIFI_AP_SSID)) {
    Serial.println("Failed to create access point");
  }

  // connect to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print(String("Connecting to wifi: \"") + WIFI_SSID + "\" with pass: \"" + WIFI_PASS + "\"");
  // Wait for connection
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    digitalWrite(PIN_LED, LED_state++ % 2);
  }

  // Start udp socket
  while(UDP.begin(8085) == 0);
  s = "";
}

void loop() {
  static int nextSendTime = 0;

  // If we recieved data from a node
  if(UDP.parsePacket()) {
    int len = UDP.read(nodebuffer, NODE_BUFFER_SIZE);
    nodebuffer[len] = '\0';

    // append it to the buffer
    s += String(nodebuffer) + "\n";
    
    Serial.println(nodebuffer);
  }

  // Enough RELAY_BUFFERING_TIME_MS has passed AND there is data in the buffer AND were connected to the internet
  if(nextSendTime - (int)millis() < 0 && s.length() > 0 && WiFi.isConnected()) {
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
      }
    }
    client.stop();

    // reset buffer
    s = "";
  }
}