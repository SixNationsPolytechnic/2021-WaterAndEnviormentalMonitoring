/*
 * This section deals with actually sending collected data off to the server through WiFi or LoRA.
 */

#include <PubSubClient.h>
#include <CayenneLPP.h>
#include <ArduinoJson.h>

DynamicJsonDoc doc(1024);

void sendToServer() {
  doc["deviceid"] = deviceInfo.devId;
  doc["latitude"] = gps_latitude();
  doc["longitude"] = gps_longitude();
  doc["msg"] = msg;
  if (WiFi.status() == WL_CONNECTED) {
    doc["mac"] = WiFi.macAddress();
    doc["ip"] = ipString();
  }
  // time
  if (deviceInfo.dht) doc["dhtTemperature"] = dhtTemp;
  if (deviceInfo.dht) doc["dhtHumidity"] = dhtHum;
  if (deviceInfo.dallas) doc["dalTemperature"] = dalTemp;
  // if any other sensor
  

  doc.clear();
}
