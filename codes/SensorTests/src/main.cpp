#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  while(!Serial);    // time to get serial running
  Serial.println(F("BME280 test"));
  delay(1000);// for uploads
    Serial.println(F("BME280 test"));
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);// for uploads
    Serial.println(F("BME280 test"));
}
