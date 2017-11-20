#include "wifi.h"

WiFiUDP client;
IPAddress recieverIP;

uint8_t MACAddr[6];
char macstr[18];

void setup_wifi() {

    WiFi.begin(WIFI_SSID);

    // Wait for connection
    while(WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
      // digitalWrite(PIN_LED, state++ % 2);
    }

  recieverIP = WiFi.gatewayIP();

  Serial.println(recieverIP);


  WiFi.macAddress(MACAddr);
  sprintf(macstr, "%02X:%02X:%02X:%02X:%02X:%02X", MACAddr[0], MACAddr[1], MACAddr[2], MACAddr[3], MACAddr[4], MACAddr[5]);
  
  client.begin(8085);
}

void wifi_write(String str) {
  client.beginPacket(recieverIP, 8085);
  client.write(str.c_str(), str.length());
  client.endPacket();
}
