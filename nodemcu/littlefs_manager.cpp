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

// Quick Actions Management Functions

String getQuickActionsFilename(String os) {
  String filename = os;
  filename.replace(" ", "_");
  filename.replace("/", "_");
  filename.replace("\\", "_");
  filename.replace("..", "_");
  return String("/quickactions_") + filename + ".txt";
}

bool saveQuickAction(String os, String cmd, String label, String desc, String btnClass) {
  if (!littlefsAvailable) {
    Serial.println("LittleFS not available for saving quick action");
    return false;
  }

  String filename = getQuickActionsFilename(os);

  // Load existing actions
  String content = "";
  if (LittleFS.exists(filename)) {
    File file = LittleFS.open(filename, "r");
    if (file) {
      while (file.available()) {
        content += (char)file.read();
      }
      file.close();
    }
  }

  // Check if action already exists and remove it
  String newContent = "";
  int startPos = 0;
  int endPos;
  while ((endPos = content.indexOf('\n', startPos)) != -1) {
    String line = content.substring(startPos, endPos);
    int firstPipe = line.indexOf('|');
    if (firstPipe > 0) {
      String existingCmd = line.substring(0, firstPipe);
      if (existingCmd != cmd) {
        newContent += line + "\n";
      }
    }
    startPos = endPos + 1;
  }
  // Handle last line without newline
  if (startPos < content.length()) {
    String line = content.substring(startPos);
    int firstPipe = line.indexOf('|');
    if (firstPipe > 0) {
      String existingCmd = line.substring(0, firstPipe);
      if (existingCmd != cmd) {
        newContent += line + "\n";
      }
    }
  }

  // Add new action
  newContent += cmd + "|" + label + "|" + desc + "|" + btnClass + "\n";

  // Save to file
  File file = LittleFS.open(filename, "w");
  if (!file) {
    Serial.println("Failed to open file for writing: " + filename);
    return false;
  }

  file.print(newContent);
  file.close();

  Serial.println("Quick action saved for " + os + ": " + cmd);
  return true;
}

String loadQuickActions(String os) {
  if (!littlefsAvailable) {
    return "";
  }

  String filename = getQuickActionsFilename(os);

  if (!LittleFS.exists(filename)) {
    return "";
  }

  File file = LittleFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open file for reading: " + filename);
    return "";
  }

  String content = "";
  while (file.available()) {
    content += (char)file.read();
  }
  file.close();

  return content;
}

bool deleteQuickAction(String os, String cmd) {
  if (!littlefsAvailable) {
    Serial.println("LittleFS not available for deleting quick action");
    return false;
  }

  String filename = getQuickActionsFilename(os);

  if (!LittleFS.exists(filename)) {
    return false;
  }

  // Load existing actions
  File file = LittleFS.open(filename, "r");
  if (!file) {
    return false;
  }

  String content = "";
  while (file.available()) {
    content += (char)file.read();
  }
  file.close();

  // Remove the action
  String newContent = "";
  bool found = false;
  int startPos = 0;
  int endPos;
  while ((endPos = content.indexOf('\n', startPos)) != -1) {
    String line = content.substring(startPos, endPos);
    int firstPipe = line.indexOf('|');
    if (firstPipe > 0) {
      String existingCmd = line.substring(0, firstPipe);
      if (existingCmd == cmd) {
        found = true;
      } else {
        newContent += line + "\n";
      }
    }
    startPos = endPos + 1;
  }
  // Handle last line
  if (startPos < content.length()) {
    String line = content.substring(startPos);
    int firstPipe = line.indexOf('|');
    if (firstPipe > 0) {
      String existingCmd = line.substring(0, firstPipe);
      if (existingCmd == cmd) {
        found = true;
      } else {
        newContent += line + "\n";
      }
    }
  }

  if (!found) {
    return false;
  }

  // Save back to file
  file = LittleFS.open(filename, "w");
  if (!file) {
    return false;
  }

  file.print(newContent);
  file.close();

  Serial.println("Quick action deleted from " + os + ": " + cmd);
  return true;
}

bool deleteAllQuickActions(String os) {
  if (!littlefsAvailable) {
    Serial.println("LittleFS not available");
    return false;
  }

  String filename = getQuickActionsFilename(os);

  if (LittleFS.exists(filename)) {
    LittleFS.remove(filename);
    Serial.println("All quick actions deleted for: " + os);
    return true;
  }

  return false;
}

// Custom OS Management Functions

bool addCustomOS(String osName) {
  if (!littlefsAvailable) {
    Serial.println("LittleFS not available for saving custom OS");
    return false;
  }

  String filename = "/customos.txt";

  // Load existing OS list
  String content = "";
  if (LittleFS.exists(filename)) {
    File file = LittleFS.open(filename, "r");
    if (file) {
      while (file.available()) {
        content += (char)file.read();
      }
      file.close();
    }
  }

  // Check if OS already exists
  int startPos = 0;
  int endPos;
  while ((endPos = content.indexOf('\n', startPos)) != -1) {
    String line = content.substring(startPos, endPos);
    if (line == osName) {
      return true; // Already exists
    }
    startPos = endPos + 1;
  }
  // Check last line
  if (startPos < content.length()) {
    String line = content.substring(startPos);
    if (line == osName) {
      return true;
    }
  }

  // Add new OS
  content += osName + "\n";

  // Save to file
  File file = LittleFS.open(filename, "w");
  if (!file) {
    Serial.println("Failed to open file for writing: " + filename);
    return false;
  }

  file.print(content);
  file.close();

  Serial.println("Custom OS added: " + osName);
  return true;
}

bool deleteCustomOS(String osName) {
  if (!littlefsAvailable) {
    Serial.println("LittleFS not available for deleting custom OS");
    return false;
  }

  String filename = "/customos.txt";

  if (!LittleFS.exists(filename)) {
    return false;
  }

  // Load existing OS list
  File file = LittleFS.open(filename, "r");
  if (!file) {
    return false;
  }

  String content = "";
  while (file.available()) {
    content += (char)file.read();
  }
  file.close();

  // Remove the OS
  String newContent = "";
  bool found = false;
  int startPos = 0;
  int endPos;
  while ((endPos = content.indexOf('\n', startPos)) != -1) {
    String line = content.substring(startPos, endPos);
    if (line == osName) {
      found = true;
    } else if (line.length() > 0) {
      newContent += line + "\n";
    }
    startPos = endPos + 1;
  }
  // Handle last line
  if (startPos < content.length()) {
    String line = content.substring(startPos);
    if (line == osName) {
      found = true;
    } else if (line.length() > 0) {
      newContent += line + "\n";
    }
  }

  if (!found) {
    return false;
  }

  // Save back to file
  file = LittleFS.open(filename, "w");
  if (!file) {
    return false;
  }

  file.print(newContent);
  file.close();

  // Also delete all quick actions for this OS
  deleteAllQuickActions(osName);

  Serial.println("Custom OS deleted: " + osName);
  return true;
}

String loadCustomOSList() {
  if (!littlefsAvailable) {
    return "";
  }

  String filename = "/customos.txt";

  if (!LittleFS.exists(filename)) {
    return "";
  }

  File file = LittleFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open file for reading: " + filename);
    return "";
  }

  String content = "";
  while (file.available()) {
    content += (char)file.read();
  }
  file.close();

  return content;
}
