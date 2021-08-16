/*
 * Read and write from the flash storage as though it were EEPROM for configured settings.
 */

#include <EEPROM.h>

void loadFlash() {
  EEPROM.begin(1024); // the EEPROM stores only devInfo - has some extra space just in case
  EEPROM.get(0, deviceInfo); // before loading firmware, the device should have its flash memory loaded with the device info
}

void writeFlash() {
  EEPROM.put(0, deviceInfo);
  EEPROM.commit();
};
