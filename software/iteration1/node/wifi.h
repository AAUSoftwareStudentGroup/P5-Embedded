#ifndef __WIFI__
#define __WIFI__

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <stdlib.h>

#include "config.h"

extern uint8_t MACAddr[6];
extern char macstr[18];

void setup_wifi();

void wifi_write(String str);
void wifi_write(char* arr, int length);
void wifi_read(); // only updates remoteIP TODO: i think?
int wifi_read(char* buffer, int length);

void wifi_setReceiverIP(IPAddress newIP);
IPAddress wifi_remoteIP();
IPAddress wifi_localIP();

#endif