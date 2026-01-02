#ifndef LITTLEFS_MANAGER_H
#define LITTLEFS_MANAGER_H

#include <Arduino.h>

void setupLittleFS();
bool saveScriptToFile(String name, String script);
String loadScriptFromFile(String name);
bool deleteScriptFile(String name);
String getScriptNameFromFilename(String filename);
String getScriptFilename(String name);


extern bool littlefsAvailable;

#endif //LITTLEFS_MANAGER_H
