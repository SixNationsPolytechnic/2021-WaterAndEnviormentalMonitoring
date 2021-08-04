/*
 * Deals with Dallas temperature sensors.
 */
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(13);
DallasTemperature sensors(&oneWire);

void dalStart() {sensors.begin();}

float dalTemp(int index = 0) {
  sensors.requestTemperatures(); // later on store last time this was called and only do it every 5s or so incase of multiple probes.
  return sensors.getTempCByIndex(index);
}
