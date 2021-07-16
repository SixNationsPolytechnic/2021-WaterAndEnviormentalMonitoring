// before loading firmware, the device should have its flash memory loaded with the device info
// use this to read battery level: https://github.com/LilyGO/TTGO-T-Beam/issues/38
// use this to get long/lat: https://github.com/LilyGO/TTGO-T-Beam/blob/master/GPS/GPS.ino
// use ESP32 deep sleep for LPM
// wifi.macAddress() to get mac

#include <EEPROM.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
//#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <PubSubClient.h>
//#include <NTPClient.h>
#include <ArduinoJson.h>
#include "CayenneLPP.h"
#include "cert.h"

String ver = {"0.0.2"}; // used for checking for updates -- only changes when updated
#define URL_version "https://raw.githubusercontent.com/SixNationsPolytechnic/2021-WaterAndEnviromentalMonitoring/Jarrod/codes/otaplayground/esp32/version.txt"
#define URL_bin "https://raw.githubusercontent.com/SixNationsPolytechnic/2021-WaterAndEnviromentalMonitoring/Jarrod/codes/otaplayground/esp32/firmware.bin"

// https://www.esp32.com/viewtopic.php?t=4931 -- implement later to make RTC survive update, if neccessary.
RTC_DATA_ATTR int message = 0; // use RTC to store data - 8kb available, survives deep sleep, doesn't harm the flash.
RTC_DATA_ATTR float temperature = 0;
RTC_DATA_ATTR float humidity = 0;

struct devInfo { // as all are char*, they are actually only able to be length-1 long. -- should be 224 bytes
  char devId[6];
  char mqtt[16];
  char ssidA[33];
  char passA[33];
  char ssidB[33];
  char passB[33];
  char ssidC[33];
  char passC[33];
};
devInfo deviceInfo;

const int checksPerDay = 2880; // 2880 is every minute; this is for testing. In the field this would likely be 12 or 24.

//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "us.pool.ntp.org", -18000, 6000);
String formattedDate;
String dayStamp;
String timeStamp;

void setup() {
  EEPROM.begin(224); // stores only devInfo
  EEPROM.get(0, deviceInfo); // before loading firmware, the device should have its flash memory loaded with the device info

  Serial.begin(115200);
  Serial.println("VERSION " + ver);
  Serial.print("Device ID: ");
  Serial.println(deviceInfo.devId);
  
  if (wifiGetConnection()) { // check for update on wakeup
    firmwareUpdate();
  }

  // collect sensor data
  // send sensor data
}

void loop() {
  // keep alive for ~10s to allow for any MQTT messages to come in, then go to deep sleep
  mqttclient.loop();
  if (true) { // 10s has passed since first loop
    //deep sleep
  }
}

/*-------------------------------------------------------------------*\
 * 
 *  WiFi Jazz
 * 
\*-------------------------------------------------------------------*/

bool wifiGetConnection () { // just a nice function to wrap checking and getting connection to any of the 3 wifi networks
  if (WiFi.status() == WL_CONNECTED) return true; // if connected return true
  boolean connected = false;
  if (deviceInfo.ssidA && deviceInfo.passA) connected = wifiConnect(deviceInfo.ssidA, deviceInfo.passA);
  if (!connected && deviceInfo.ssidB && deviceInfo.passB) connected = wifiConnect(deviceInfo.ssidB, deviceInfo.passB);
  if (!connected && deviceInfo.ssidC && deviceInfo.passC) connected = wifiConnect(deviceInfo.ssidC, deviceInfo.passC);
  return connected; // will be true if it was able to connect
}

bool wifiConnect (char* ssid, char* pass) { // attempt to connect to a wifi network
  int counter = 0;
  Serial.print("Attempting connection to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (counter > 20) { // only attempt to connect for 10s.
      Serial.println("\nCould not connect.");
      return false;
    } else counter++;
  }
  Serial.println("\nConnected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

String macToStr() {
  String result;
  uint8_t mac = WiFi.macAddress();
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

/*-------------------------------------------------------------------*\
 * 
 *  MQTT Jazz
 * 
\*-------------------------------------------------------------------*/

CayenneLPP lpp(51);
PubSubClient mqttclient(espClient);

void mqttSetup() {
  mqttclient.setServer(deviceInfo.mqtt, 1883);
  mqttclient.setCallback(callback);
  mqttclient.subscribe("sensor/jw01/command");
}

void mqttSend(String mqttmsg) {
  lpp.reset();
  lpp.addTemperature(1, temperature);
  lpp.addRelativeHumidity(2, temperature);
  String mytopic = "lpp/" + String(deviceInfo.devId) + "/data";
  mqttclient.publish(mytopic.c_str(), (char*)lpp.getBuffer(), true);
  message++;
}

void mqttReconnect() {
  String mqttmsg = "{\"hello\":\"hello\"}";
  while (!mqttclient.connected()) {
    String clientName;
    clientName += "esp8266-";
    clientName += macToStr();
    clientName += "-";
    clientName += String(micros() & 0xff, 16);
    Serial.print("Connecting to ");
    Serial.print(mqtt_server);
    Serial.print(" as ");
    Serial.println(clientName);

    if (mqttclient.connect((char*) clientName.c_str())) {
      //if (client.connect((char*) clientName.c_str()), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      Serial.println("Sending hello message");
      mqttmsg = "{\"hello\":\"" + String(DEVICEID) + "\"}";
      String mytopic =  "sensor/" + String(DEVICEID) + "/hello";
      mqttclient.publish(mytopic.c_str (), mqttmsg.c_str(), true);

    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttclient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String s;
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    s=s+(char)payload[i];
    
  }
  if(strstr(topic, "mqttSchedule") != NULL) {}; // use things like this to change settings
  Serial.println(s);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/*-------------------------------------------------------------------*\
 * 
 *  Firmware Update Jazz
 * 
\*-------------------------------------------------------------------*/

void firmwareUpdate() {if (firmwareUpdateCheck()) firmwareUpdateExecute();} // just makes it more readable. Essentially just checks if there's an update, and if so, dew it


bool firmwareUpdateCheck() { // https://github.com/programmer131/ESP8266_ESP32_SelfUpdate
  Serial.println("Checking for update.");
  String payload;
  int httpCode;
  String fwurl = "";
  fwurl += URL_version;
  fwurl += "?";
  fwurl += String(rand());
  //Serial.println(fwurl);
  WiFiClientSecure * client = new WiFiClientSecure;

  if (client)
  {
    client -> setCACert(rootCACertificate);

    // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
    HTTPClient https;

    if (https.begin( * client, fwurl))
    { // HTTPS
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK) // if version received
      {
        payload = https.getString(); // save received version
      } else {
        Serial.print("error in downloading version file:");
        Serial.println(httpCode);
      }
      https.end();
    }
    delete client;
  }

  if (httpCode == HTTP_CODE_OK) // if version received
  {
    payload.trim();
    if (payload.equals(ver)) {
      Serial.printf("\nDevice already on latest firmware version: %s\n", ver);
      return false;
    }
    else
    {
      Serial.printf("New firmware aavailable: %s\n", payload);
      return true;
    }
  }
  return false;
}

void firmwareUpdateExecute() { // https://github.com/programmer131/ESP8266_ESP32_SelfUpdate
  WiFiClientSecure client;
  client.setCACert(rootCACertificate);
  httpUpdate.setLedPin(LED_BUILTIN, LOW);
  t_httpUpdate_return ret = httpUpdate.update(client, URL_bin);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }
}
