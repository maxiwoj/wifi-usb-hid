#ifndef LITTLEFS_MANAGER_H
#define LITTLEFS_MANAGER_H

#include <Arduino.h>

void setupLittleFS();
bool saveScriptToFile(String name, String script);
String loadScriptFromFile(String name);
bool deleteScriptFile(String name);
String getScriptNameFromFilename(String filename);
String getScriptFilename(String name);

// Quick actions management
bool saveQuickAction(String os, String cmd, String label, String desc, String btnClass);
String loadQuickActions(String os);
bool deleteQuickAction(String os, String cmd);
bool deleteAllQuickActions(String os);

// Custom OS management
bool addCustomOS(String osName);
bool deleteCustomOS(String osName);
String loadCustomOSList();

// Quick scripts management
bool saveQuickScript(String os, String id, String label, String script, String btnClass);
String loadQuickScripts(String os);
bool deleteQuickScript(String os, String id);
bool deleteAllQuickScripts(String os);

extern bool littlefsAvailable;

#endif //LITTLEFS_MANAGER_H
