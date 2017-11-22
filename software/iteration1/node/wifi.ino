#include "wifi.h"

WiFiUDP client;
IPAddress recieverIP;

uint8_t MACAddr[6];
char macstr[18];

void setup_wifi() {
  IPAddress defaultIP = client.remoteIP();

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

void wifi_write(char* arr, int length) {
  client.beginPacket(recieverIP, 8085);
  client.write(arr, length);
  client.endPacket();
}

// Returns: Number of written bytes to buffer. 0 if no packet is available
int wifi_read(char* buffer, int length) {
  if(client.parsePacket() > 0) {
    return client.read(buffer, length);
  }
  return 0;
}

// Returns: Number of written bytes to buffer. 0 if no packet is available
void wifi_read() {
  client.parsePacket();
}

IPAddress wifi_remoteIP() {
  return client.remoteIP();
}

IPAddress wifi_localIP() {
  return WiFi.localIP();
}

void wifi_setReceiverIP(IPAddress newIP) {
  recieverIP = newIP;
}