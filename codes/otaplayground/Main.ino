/*
 * Environment Sensor Project
 * Jarrod Wardell
 * 
 * Aug. 3, 2021 - Updated Aug. 13, 2021
 */
String ver = "2021-08-18-1";
#define debug false

struct devInfo {
  char devId[6];
  char mqtt[16];
  char ssidA[33];
  char passA[33];
  char ssidB[33];
  char passB[33];
  char ssidC[33];
  char passC[33];
  int sleepLength;
  bool dallas;
  bool dht;
  bool bmp;
  bool screen;
  bool mq2;
  bool mq3;
  bool mq4;
  bool mq6;
  bool mq7;
  bool mq8;
  bool mq9;
  bool mq135;
  bool distance;
  bool moisture;
  bool light;
  bool noise;
};
devInfo deviceInfo;

void setup() {
  Serial.begin(115200);
  loadFlash();
  delay(1000);
  if (deviceInfo.screen) displayStart();
  if (!wifiGetConnection()) {
    goToSleep();
  }
  firmwareUpdate();
  gps_setup();
  dalStart();
  dhtStart();
  mqttStart();
  delay(2000);
  gps_loop();
  collectAndSend();
}

void loop() {
  
}
