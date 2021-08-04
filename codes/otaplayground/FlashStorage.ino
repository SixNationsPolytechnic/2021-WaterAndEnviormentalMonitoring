/*
 * Read and write from the flash storage as though it were EEPROM for configured settings.
 */

#include <EEPROM.h>

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

void loadFlash() {
  EEPROM.begin(224); // the EEPROM stores only devInfo
  EEPROM.get(0, deviceInfo); // before loading firmware, the device should have its flash memory loaded with the device info
}

void writeFlash() {};
