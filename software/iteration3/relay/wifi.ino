#include "wifi.h"

WiFiUDP UDP;
IPAddress recieverIP;

uint8_t MACAddr[6];
char macstr[18];

void setup_wifi() {
  WiFi.mode(WIFI_STA);
  
  #if WIFI_USE_PASS == true
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  #else
  WiFi.begin(WIFI_SSID);
  #endif

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
  
  // Start listening on port 8085
  UDP.begin(UDP_PORT);
}

void wifi_handle_data() {
  if (UDP.parsePacket() > 0) {
    // Change reciever of data to the one who just contacted us
    recieverIP = UDP.remoteIP();



    // int len = UDP.read(incomingPacket, 255);
    // if (len > 0) {
    //   incomingPacket[len] = 0;
    // }
  }
}

void wifi_write(String str) {
  UDP.beginPacket(recieverIP, UDP_PORT);
  UDP.write(str.c_str(), str.length());
  UDP.endPacket();
}
