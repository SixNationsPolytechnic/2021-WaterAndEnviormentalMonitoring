/*
 * Environment Sensor Project
 * Jarrod Wardell
 * 
 * Aug. 3, 2021 - Updated Aug. 4, 2021
 */
String ver = "2021.08.04-0";
#define debug false

int msg = 1;

void setup() {
  displayStart();
  delay(1000);
  loadFlash();
  wifiGetConnection();
  firmwareUpdate();
  gps_setup();
  gps_loop();
  dalStart();
  dhtStart();
  mqttStart();
  delay(1000);
}

void loop() {
  
}
