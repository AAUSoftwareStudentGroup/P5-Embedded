#include "wifi.h"

WiFiUDP client;
IPAddress recieverIP;


void setup_wifi() {
  IPAddress defaultIP = client.remoteIP();

// Connect to wifi network
  #ifdef CONFIG_WIFI_AS_ACCESSPOINT
  while(!WiFi.softAP(WIFI_AP_SSID)) {
    Serial.println("Failed to create AP");
  }
  #else
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to wifi");
  // Wait for connection
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    digitalWrite(PIN_LED, state++ % 2);
  }

  // Print the IP address to serial
  Serial.println(WiFi.localIP());
  #endif


  client.begin(8085);
  while(client.remoteIP() == defaultIP) {
    delay(250);
    Serial.print(".");
    // digitalWrite(LED, state++ % 2);
  }
  recieverIP = client.remoteIP();
}

void wifi_write(String str) {
  client.beginPacket(recieverIP, 8085);
  client.write(str.c_str(), str.length());
  client.endPacket();
}
