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

#endif