#include "SSD1306.h"

SSD1306 display(0x3c, 21, 22);

void setup() {
  Serial.begin(115200);
  Serial.println"Hello World");
  display.init();

  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 0, "Hello World");
  display.display();
}

void loop() {

}
