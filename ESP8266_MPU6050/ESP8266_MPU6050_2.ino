#include <I2Cdev.h>
#include <SPI.h>
#include <Wire.h>

//#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

// If using software SPI (the default case):
#define OLED_MOSI  4
#define OLED_CLK   5
#define OLED_DC    0
#define OLED_CS    2
#define OLED_RESET 16
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int i = 0;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC); // No idea what it does, does not work without
  display.setTextSize(1);
  display.setTextColor(WHITE);
}


void loop() { 
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(i++);
  display.display();
}
