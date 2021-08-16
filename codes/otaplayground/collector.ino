void goToSleep() {
  esp_sleep_enable_timer_wakeup(deviceInfo.sleepLength * 60 * 1000000); // sleep for time in minutes
  Serial.print("Sleeping for ");
  Serial.print(deviceInfo.sleepLength);
  Serial.print(" minutes.");
  Serial.flush(); 
  esp_deep_sleep_start();
}

void collectAndSend() {
  if (deviceInfo.dht) dhtTempRead();
  if (deviceInfo.dht) dhtHumRead();
  if (deviceInfo.dallas) dalTempRead();

  sendDataServer();

  goToSleep();
}
