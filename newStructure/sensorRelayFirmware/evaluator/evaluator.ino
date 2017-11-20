#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <stdlib.h>

#define WIFI_AP_SSID "FootballRelay"
#define WIFI_SSID "wifi"
#define WIFI_PASS "TannerHelland"
#define UDP_PORT 8085
#define HTTP_HOST "p5datahub.azurewebsites.net"
#define HTTP_PORT 80

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

	WiFi.mode(WIFI_AP_STA);
	while(!WiFi.softAP(WIFI_AP_SSID)) {
    Serial.println("Failed to create AP");
	}

	// connect to wifi
	WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to wifi");
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

	// Serial.println(WiFi.softAPgetStationNum());
	if(UDP.parsePacket()) {
		Serial.println("Got data");
		int len = UDP.read(nodebuffer, NODE_BUFFER_SIZE);
		nodebuffer[len] = '\0';
		s += String(nodebuffer) + "\n";
	}

	if(nextSendTime - (int)millis() < 0 && s.length() > 0 && WiFi.isConnected()) {
		nextSendTime = millis()+3000;

		Serial.println("Sending data");
		// send data
	  if (!client.connect(HTTP_HOST, HTTP_PORT)) {
	    Serial.println("connection failed");
	    return;
	  }
		Serial.println("Connected to server");
	 
	  client.print(String("POST") + " /api/sensor/data HTTP/1.1\r\n" \
	               "Host: " + HTTP_HOST + "\r\n" \
	               "Content-Type: application/json\r\n" \
	               "Content-Length: "+ (s.length()+2) +"\r\n" \
	               "Connection: close\r\n\r\n"+"\"" + s + "\"");

		Serial.println("Waiting for response...");
		int timeout = millis()+2000;
	  while(client.connected()) {
	  	if (timeout-(int)millis() < 0) {
	      Serial.println("Client Timeout !");
	      break;
	    }
		  if(client.available()){
		    String line = client.readStringUntil('\n');
		   
		   Serial.println(line);
		  }
	  }
	  client.stop();

		Serial.println("DONE!!");
		Serial.println();
		Serial.println();

		// reset buffer
		s = "";
	}
}