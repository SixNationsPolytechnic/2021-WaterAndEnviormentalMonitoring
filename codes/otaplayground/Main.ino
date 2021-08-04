/*
 * Environment Sensor Project
 * Jarrod Wardell
 * 
 * Aug. 3, 2021 - Updated Aug. 4, 2021
 */
 #define ver = "2021.08.04-0";
 #define debug = false;

void setup() {
  loadFlash();
  wifiGetConnection ();
  firmwareUpdate();
  gps_setup();
  dalStart();
  dhtStart();
  displayStart();
}

void loop() {
  // put your main code here, to run repeatedly:

}
