#include "wifi.h"

WiFiUDP client;
IPAddress recieverIP;

uint8_t MACAddr[6];
char macstr[18];

void setup_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID);

  // Wait for connection
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Set the reciever IP to the accesspoint ip
  recieverIP = WiFi.gatewayIP();

  // Read MAC address
  WiFi.macAddress(MACAddr);
  sprintf(macstr, "%02X:%02X:%02X:%02X:%02X:%02X", MACAddr[0], MACAddr[1], MACAddr[2], MACAddr[3], MACAddr[4], MACAddr[5]);
  
  // Start listening on port 8085s
  client.begin(UDP_PORT);
}

void wifi_write(String str) {
  client.beginPacket(recieverIP, 8085);
  client.write(str.c_str(), str.length());
  client.endPacket();
}
