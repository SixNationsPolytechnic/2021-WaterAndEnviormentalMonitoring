#include <Wire.h>
#include "DHT.h";

DHT dht(14, DHT11);

void dhtStart() {dht.begin();}

float dhtTemp = 0;
float dhtHum = 0;

float dhtTemp() {
  dhtTemp = dht.readTemperature();
  return dhtTemp;
}
float dhtHum() {
  dhtHum = dht.readHumidity();
  return dhtHum;
}
