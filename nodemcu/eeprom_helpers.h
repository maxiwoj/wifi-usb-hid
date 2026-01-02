#ifndef EEPROM_HELPERS_H
#define EEPROM_HELPERS_H

#include <Arduino.h>

String readStringFromEEPROM(int addr, int maxLen);
void writeStringToEEPROM(int addr, String data, int maxLen);

#endif //EEPROM_HELPERS_H
