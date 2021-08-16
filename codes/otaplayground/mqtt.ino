/*
 * This section deals with actually sending collected data off to the server through WiFi
 */

#include <PubSubClient.h>
#include <CayenneLPP.h>
#include <ArduinoJson.h>

DynamicJsonDocument doc(1024);
RTC_DATA_ATTR int messages;
bool hasMessage = false;

WiFiClient espClient;
PubSubClient client(espClient);


void repeat() {
  int looper = 0;
  while (!hasMessage && 100 <= looper) {
    if (!client.connected()) reconnect();
    client.loop();
    looper++;
    delay(100);
  }
  return;
}

/* Send data to server */
void sendDataServer() {
  doc["deviceid"] = deviceInfo.devId;
  doc["version"] = ver;
  doc["latitude"] = gps_latitude();
  doc["longitude"] = gps_longitude();
  doc["msg"] = messages;
  if (WiFi.status() == WL_CONNECTED) {
    doc["mac"] = WiFi.macAddress();
    doc["ip"] = ipString();
  }
  // time
  if (deviceInfo.dht) doc["dhtTemperature"] = dhtTemp;
  if (deviceInfo.dht) doc["dhtHumidity"] = dhtHum;
  if (deviceInfo.dallas) doc["dalTemperature"] = dalTemp;
  // if any other sensor
  
  char docBuffer[1024];
  size_t n = serializeJson(doc, docBuffer);
  char toSend[40] = "sensor/";
  strcat(toSend, deviceInfo.devId);
  strcat(toSend, "/data");
  client.publish(toSend, docBuffer, n);
  doc.clear();
  println("Awaiting response");
  repeat();
  return;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      char topic[50];
      strcpy(topic, String("sensor/" + String(deviceInfo.devId) + "/command").c_str());
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/* Receive messages back from the server, for updating or for acknowledging receival of data */
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println();
}

void mqttStart() {
  client.setBufferSize(1024);
  client.setServer(deviceInfo.mqtt, 1883);
  client.setCallback(callback);
  reconnect();
}
