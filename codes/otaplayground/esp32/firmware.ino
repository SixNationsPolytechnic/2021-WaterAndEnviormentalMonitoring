#include <EEPROM.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
//#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
//#include <PubSubClient.h>
//#include <NTPClient.h>
//#include <ArduinoJson.h>
//#include "CayenneLPP.h"
#include "cert.h"

String ver = {"0.0.1"}; // used for checking for updates -- only changes when updated
#define URL_version "https://raw.githubusercontent.com/SixNationsPolytechnic/2021-WaterAndEnviromentalMonitoring/Jarrod/codes/otaplayground/esp32/version.txt"
#define URL_bin "https://raw.githubusercontent.com/SixNationsPolytechnic/2021-WaterAndEnviromentalMonitoring/Jarrod/codes/otaplayground/esp32/firmware.bin"

// https://www.esp32.com/viewtopic.php?t=4931 -- implement later to make RTC survive update, if neccessary.
//RTC_DATA_ATTR char* devId = "jw00T"; // use RTC to store data - 8kb available, survives deep sleep.

struct deviceInfo {
  char devId[6];
  char mqtt[16];
  char ssidA[33];
  char passA[33];
  char ssidB[33];
  char passB[33];
  char ssidC[33];
  char passC[33];
};



//CayenneLPP lpp(51);
//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "us.pool.ntp.org", -18000, 6000);
String formattedDate;
String dayStamp;
String timeStamp;

void setup() {
  deviceInfo d;
  strcpy(d.devId, "jw00T");
  strcpy(d.mqtt, "52.117.240.201");
  strcpy(d.ssidA, "Aw geez Rick theres a password");
  strcpy(d.passA, "SpaceRoses13");
  strcpy(d.ssidB, "WSPDev");
  strcpy(d.passB, "InTh3F1eld!");
  strcpy(d.ssidC, "Network3");
  strcpy(d.passC, "Example1793#.");
  EEPROM.begin(224); // the deviceInfo struct should only be 224 bytes
  EEPROM.put(0, d);
  EEPROM.commit();

  

  Serial.begin(115200);
  Serial.println("VERSION " + ver);
  boolean connected = false;
  //if (deviceInfo.ssidA && deviceInfo.passA) connected = wifiConnect(deviceInfo.ssidA, deviceInfo.passA);
  //if (!connected && deviceInfo.ssidB && deviceInfo.passB) connected = wifiConnect(deviceInfo.ssidB, deviceInfo.passB);
  //if (!connected && deviceInfo.ssidC && deviceInfo.passC) connected = wifiConnect(deviceInfo.ssidC, deviceInfo.passC);
  if (!connected) {

  } else {
    //firmwareUpdate();
  }
}

void loop() {
  delay(10000);
  //firmwareUpdate();
}

bool wifiConnect (char* ssid, char* pass) {
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

void firmwareUpdate() {if (firmwareUpdateCheck()) firmwareUpdateExecute();}

bool firmwareUpdateCheck() {
  Serial.println("Checking for update.");
  String payload;
  int httpCode;
  String fwurl = "";
  fwurl += URL_version;
  fwurl += "?";
  fwurl += String(rand());
  Serial.println(fwurl);
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
      Serial.printf("\nDevice already on latest firmware version:%s\n", ver);
      return false;
    }
    else
    {
      Serial.println(payload);
      Serial.println("New firmware detected");
      return true;
    }
  }
  return false;
}

void firmwareUpdateExecute() {
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
