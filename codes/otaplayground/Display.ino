#include <Wire.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 32, &Wire);

void displayClear() {
  if (!deviceInfo.screen) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
}

void displayStart() {
  if (!deviceInfo.screen) return;
  display.begin();
  displayClear();
}

void print(String toPrint) {
  Serial.print(toPrint);
  if (deviceInfo.screen) display.print(toPrint);
}

void println(String toPrint) {
  Serial.println(toPrint);
  if (deviceInfo.screen) display.println(toPrint);
}

void output() {
  if (!deviceInfo.screen) return;
  display.display();
  }
