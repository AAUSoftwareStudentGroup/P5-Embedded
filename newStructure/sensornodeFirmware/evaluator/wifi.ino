#include "wifi.h"

WiFiUDP client;
IPAddress recieverIP;


void setup_wifi() {
  IPAddress defaultIP = client.remoteIP();

// Connect to wifi network
// WiFi.begin(ssid, password);
  while(!WiFi.softAP(ssid)) {
    Serial.println("Failed to create AP");
  }

//   Serial.print("Connecting to wifi");
//   // Wait for connection
//   while(WiFi.status() != WL_CONNECTED) {
//     Serial.print(".");
//     delay(500);
//     // digitalWrite(LED, state++ % 2);
//   }

  Serial.println();
  Serial.println("Waiting for app to say hi");

  // Print the IP address to serial
  // Serial.println(WiFi.localIP());

  client.begin(8085);
  // client.beginPacket(client.remoteIP(), 8085);
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
