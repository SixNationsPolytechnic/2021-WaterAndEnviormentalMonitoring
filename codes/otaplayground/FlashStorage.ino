/*
 * Read and write from the flash storage as though it were EEPROM for configured settings.
 */

#include <EEPROM.h>

struct devInfo {
  char devId[6];
  char mqtt[16];
  char ssidA[33];
  char passA[33];
  char ssidB[33];
  char passB[33];
  char ssidC[33];
  char passC[33];
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
  bool waterLevel;
  bool moisture;
  bool light;
  bool noise;
};
devInfo deviceInfo;

void loadFlash() {
  EEPROM.begin(500); // the EEPROM stores only devInfo - has some extra space just in case
  EEPROM.get(0, deviceInfo); // before loading firmware, the device should have its flash memory loaded with the device info
}

void writeFlash() {};
