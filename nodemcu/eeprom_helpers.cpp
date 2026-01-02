#include "eeprom_helpers.h"
#include <EEPROM.h>

String readStringFromEEPROM(int addr, int maxLen) {
  String result = "";
  for (int i = 0; i < maxLen; i++) {
    char c = EEPROM.read(addr + i);
    if (c == 0) break;
    result += c;
  }
  return result;
}

void writeStringToEEPROM(int addr, String data, int maxLen) {
  int len = data.length();
  if (len > maxLen) len = maxLen;

  for (int i = 0; i < len; i++) {
    EEPROM.write(addr + i, data[i]);
  }
  // Null terminate
  if (len < maxLen) {
    EEPROM.write(addr + len, 0);
  }
}
