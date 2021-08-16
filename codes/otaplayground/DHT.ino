#include <Wire.h>
#include "DHT.h";

DHT dht(14, DHT11);

void dhtStart() {
  dht.begin();
}

float dhtTemp = 0.0;
float dhtHum = 0.0;

float dhtTempRead() {
  dhtTemp = dht.readTemperature();
  return dhtTemp;
}
float dhtHumRead() {
  dhtHum = dht.readHumidity();
  return dhtHum;
}
