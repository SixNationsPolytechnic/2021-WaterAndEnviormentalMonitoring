#include <Wire.h>
#include "DHT.h";

DHT dht(14, DHT11);

void dhtStart() {dht.begin();}

float dhtTemp() {return dht.readTemperature();}
float dhtHum() {return dht.readHumidity();}
