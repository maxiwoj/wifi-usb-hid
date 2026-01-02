#include "littlefs_manager.h"
#include <LittleFS.h>
#include "config.h"

bool littlefsAvailable = false;

void setupLittleFS() {
    littlefsAvailable = LittleFS.begin();
    if (littlefsAvailable) {
        Serial.println("LittleFS mounted successfully");
        
        // Check LittleFS info
        FSInfo fs_info;
        LittleFS.info(fs_info);
        Serial.print("LittleFS Total bytes: ");
        Serial.println(fs_info.totalBytes);
        Serial.print("LittleFS Used bytes: ");
        Serial.println(fs_info.usedBytes);

        Serial.println("LittleFS contents:");
        Dir dir = LittleFS.openDir("/");
        while (dir.next()) {
        Serial.println(dir.fileName());
        }
    } else {
        Serial.println("LittleFS initialization failed - script storage will not be available");
        Serial.println("To enable LittleFS: Tools > ESP8266 Sketch Data Upload in Arduino IDE");
        Serial.println("Server will continue without LittleFS support");
    }
}

String getScriptFilename(String name) {
  // Sanitize filename - remove invalid characters
  String filename = name;
  filename.replace(" ", "_");
  filename.replace("/", "_");
  filename.replace("\\", "_");
  filename.replace("..", "_");

  // Truncate if too long
  if (filename.length() > MAX_SCRIPT_NAME_LEN) {
    filename = filename.substring(0, MAX_SCRIPT_NAME_LEN);
  }

  return String("/scripts_") + filename + ".txt";
}

bool saveScriptToFile(String name, String script) {
  if (!littlefsAvailable) {
    Serial.println("LittleFS not available for saving script");
    return false;
  }

  String filename = getScriptFilename(name);

  File file = LittleFS.open(filename, "w");
  if (!file) {
    Serial.println("Failed to open file for writing: " + filename);
    return false;
  }

  // Write script content
  file.print(script);
  file.close();

  Serial.println("Script saved: " + filename);
  return true;
}

String loadScriptFromFile(String name) {
  if (!littlefsAvailable) {
    Serial.println("LittleFS not available for loading script");
    return "";
  }

  String filename = getScriptFilename(name);

  File file = LittleFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open file for reading: " + filename);
    return "";
  }

  String script = "";
  while (file.available()) {
    script += (char)file.read();
  }
  file.close();

  return script;
}

bool deleteScriptFile(String name) {
  if (!littlefsAvailable) {
    Serial.println("LittleFS not available for deleting script");
    return false;
  }

  String filename = getScriptFilename(name);

  if (LittleFS.exists(filename)) {
    LittleFS.remove(filename);
    Serial.println("Script deleted: " + filename);
    return true;
  }

  return false;
}

String getScriptNameFromFilename(String filename) {
  // Extract name from "/scripts_<name>.txt"
  if (filename.startsWith("scripts_") && filename.endsWith(".txt")) {
    String name = filename.substring(8); // Skip "/scripts_"
    name = name.substring(0, name.length() - 4); // Remove ".txt"
    name.replace("_", " ");
    return name;
  }
  return "";
}
