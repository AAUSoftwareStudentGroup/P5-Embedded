#include "wifi.h"

WiFiUDP client;
IPAddress recieverIP;

uint8_t MACAddr[6];
char macstr[18];

void setup_wifi() {
  IPAddress defaultIP = client.remoteIP();

  #ifdef CONFIG_WIFI_AS_ACCESSPOINT
  
    // start AP
    while(!WiFi.softAP(WIFI_AP_SSID)) {
      Serial.println("Failed to create AP");
    }
  #endif
  #ifdef CONFIG_WIFI_AS_NODE
    // Connect to wifi network
    
    #if WIFI_USE_PASS == true
      WiFi.begin(WIFI_SSID, WIFI_PASS);
    #else 
      WiFi.begin(WIFI_SSID);
    #endif

    // Wait for connection
    while(WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
      // digitalWrite(PIN_LED, state++ % 2);
    }
  #endif

  recieverIP = WiFi.gatewayIP();


  WiFi.macAddress(MACAddr);
  sprintf(macstr, "%02X:%02X:%02X:%02X:%02X:%02X", MACAddr[0], MACAddr[1], MACAddr[2], MACAddr[3], MACAddr[4], MACAddr[5]);
  
  client.begin(8085);
}

void wifi_write(String str) {
  client.beginPacket(recieverIP, 8085);
  client.write(str.c_str(), str.length());
  client.endPacket();
}
