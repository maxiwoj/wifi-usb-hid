#include "littlefs_manager.h"
#include <LittleFS.h>
#include <SD_MMC.h>
#include "config.h"

bool storageAvailable = false;
bool usingSD = false;
fs::FS* storageFS = nullptr;

void setupStorage() {
    // Try to initialize SD Card first
    Serial.println("Attempting to mount SD Card...");
    SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0, SD_MMC_D1, SD_MMC_D2, SD_MMC_D3);
    
    // Attempt to mount SD card (mount point /sdcard, mode 1bit=false for 4-bit, format=false)
    if (SD_MMC.begin("/sdcard", false)) {
        Serial.println("SD Card mounted successfully");
        storageAvailable = true;
        usingSD = true;
        storageFS = &SD_MMC;
        
        uint8_t cardType = SD_MMC.cardType();
        if(cardType == CARD_NONE){
             Serial.println("No SD Card attached");
             storageAvailable = false;
             usingSD = false;
        } else {
             Serial.print("SD Card Type: ");
             if(cardType == CARD_MMC) Serial.println("MMC");
             else if(cardType == CARD_SD) Serial.println("SDSC");
             else if(cardType == CARD_SDHC) Serial.println("SDHC");
             else Serial.println("UNKNOWN");
        }
    } 
    
    if (!storageAvailable) {
        Serial.println("SD Card mount failed or not present. Falling back to LittleFS...");
        
        // Fallback to LittleFS
        if (LittleFS.begin(true)) { // true = format if failed
            Serial.println("LittleFS mounted successfully");
            storageAvailable = true;
            usingSD = false;
            storageFS = &LittleFS;
        } else {
            Serial.println("LittleFS initialization failed - script storage will not be available");
            Serial.println("To enable LittleFS: Tools > ESP32 Sketch Data Upload in Arduino IDE");
            Serial.println("Server will continue without storage support");
            storageFS = nullptr;
        }
    }

    if (storageAvailable) {
        size_t total = 0, used = 0;
        getFilesystemInfo(total, used);
        Serial.print("Storage Total bytes: ");
        Serial.println(total);
        Serial.print("Storage Used bytes: ");
        Serial.println(used);

        Serial.println("Storage contents:");
        File root = storageFS->open("/");
        File file = root.openNextFile();
        while (file) {
            Serial.print("  ");
            Serial.println(file.name());
            file = root.openNextFile();
        }
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
  if (!storageAvailable || !storageFS) {
    Serial.println("Storage not available for saving script");
    return false;
  }

  String filename = getScriptFilename(name);

  File file = storageFS->open(filename, "w");
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
  if (!storageAvailable || !storageFS) {
    Serial.println("Storage not available for loading script");
    return "";
  }

  String filename = getScriptFilename(name);

  File file = storageFS->open(filename, "r");
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
  if (!storageAvailable || !storageFS) {
    Serial.println("Storage not available for deleting script");
    return false;
  }

  String filename = getScriptFilename(name);

  if (storageFS->exists(filename)) {
    storageFS->remove(filename);
    Serial.println("Script deleted: " + filename);
    return true;
  }

  return false;
}

String getScriptNameFromFilename(String filename) {
  // Extract name from "/scripts_<name>.txt"
  // Remove leading slash if present
  if (filename.startsWith("/")) filename = filename.substring(1);
  
  if (filename.startsWith("scripts_") && filename.endsWith(".txt")) {
    String name = filename.substring(8); // Skip "scripts_"
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
  if (!storageAvailable || !storageFS) {
    Serial.println("Storage not available for saving quick action");
    return false;
  }

  String filename = getQuickActionsFilename(os);

  // Load existing actions
  String content = "";
  if (storageFS->exists(filename)) {
    File file = storageFS->open(filename, "r");
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
  File file = storageFS->open(filename, "w");
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
  if (!storageAvailable || !storageFS) {
    return "";
  }

  String filename = getQuickActionsFilename(os);

  if (!storageFS->exists(filename)) {
    return "";
  }

  File file = storageFS->open(filename, "r");
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
  if (!storageAvailable || !storageFS) {
    Serial.println("Storage not available for deleting quick action");
    return false;
  }

  String filename = getQuickActionsFilename(os);

  if (!storageFS->exists(filename)) {
    return false;
  }

  // Load existing actions
  File file = storageFS->open(filename, "r");
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
  file = storageFS->open(filename, "w");
  if (!file) {
    return false;
  }

  file.print(newContent);
  file.close();

  Serial.println("Quick action deleted from " + os + ": " + cmd);
  return true;
}

bool deleteAllQuickActions(String os) {
  if (!storageAvailable || !storageFS) {
    Serial.println("Storage not available");
    return false;
  }

  String filename = getQuickActionsFilename(os);

  if (storageFS->exists(filename)) {
    storageFS->remove(filename);
    Serial.println("All quick actions deleted for: " + os);
    return true;
  }

  return false;
}

// Custom OS Management Functions

bool addCustomOS(String osName) {
  if (!storageAvailable || !storageFS) {
    Serial.println("Storage not available for saving custom OS");
    return false;
  }

  String filename = "/customos.txt";

  // Load existing OS list
  String content = "";
  if (storageFS->exists(filename)) {
    File file = storageFS->open(filename, "r");
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
  File file = storageFS->open(filename, "w");
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
  if (!storageAvailable || !storageFS) {
    Serial.println("Storage not available for deleting custom OS");
    return false;
  }

  String filename = "/customos.txt";

  if (!storageFS->exists(filename)) {
    return false;
  }

  // Load existing OS list
  File file = storageFS->open(filename, "r");
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
  file = storageFS->open(filename, "w");
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
  if (!storageAvailable || !storageFS) {
    return "";
  }

  String filename = "/customos.txt";

  if (!storageFS->exists(filename)) {
    return "";
  }

  File file = storageFS->open(filename, "r");
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

// Quick Scripts Management Functions

String getQuickScriptsFilename(String os) {
  String filename = os;
  filename.replace(" ", "_");
  filename.replace("/", "_");
  filename.replace("\\", "_");
  filename.replace("..", "_");
  return String("/quickscripts_") + filename + ".txt";
}

bool saveQuickScript(String os, String id, String label, String script, String btnClass) {
  if (!storageAvailable || !storageFS) {
    Serial.println("Storage not available for saving quick script");
    return false;
  }

  String filename = getQuickScriptsFilename(os);

  // Load existing scripts
  String content = "";
  if (storageFS->exists(filename)) {
    File file = storageFS->open(filename, "r");
    if (file) {
      while (file.available()) {
        content += (char)file.read();
      }
      file.close();
    }
  }

  // Check if script already exists and remove it
  String newContent = "";
  int startPos = 0;
  int endPos;
  while ((endPos = content.indexOf('\n', startPos)) != -1) {
    String line = content.substring(startPos, endPos);
    int firstPipe = line.indexOf('|');
    if (firstPipe > 0) {
      String existingId = line.substring(0, firstPipe);
      if (existingId != id) {
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
      String existingId = line.substring(0, firstPipe);
      if (existingId != id) {
        newContent += line + "\n";
      }
    }
  }

  // Escape newlines in script for storage
  String escapedScript = script;
  escapedScript.replace("\n", "\\n");

  // Add new script (format: id|label|script|class)
  newContent += id + "|" + label + "|" + escapedScript + "|" + btnClass + "\n";

  // Save to file
  File file = storageFS->open(filename, "w");
  if (!file) {
    Serial.println("Failed to open file for writing: " + filename);
    return false;
  }

  file.print(newContent);
  file.close();

  Serial.println("Quick script saved for " + os + ": " + id);
  return true;
}

String loadQuickScripts(String os) {
  if (!storageAvailable || !storageFS) {
    return "";
  }

  String filename = getQuickScriptsFilename(os);

  if (!storageFS->exists(filename)) {
    return "";
  }

  File file = storageFS->open(filename, "r");
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

bool deleteQuickScript(String os, String id) {
  if (!storageAvailable || !storageFS) {
    Serial.println("Storage not available for deleting quick script");
    return false;
  }

  String filename = getQuickScriptsFilename(os);

  if (!storageFS->exists(filename)) {
    return false;
  }

  // Load existing scripts
  File file = storageFS->open(filename, "r");
  if (!file) {
    return false;
  }

  String content = "";
  while (file.available()) {
    content += (char)file.read();
  }
  file.close();

  // Remove the script
  String newContent = "";
  bool found = false;
  int startPos = 0;
  int endPos;
  while ((endPos = content.indexOf('\n', startPos)) != -1) {
    String line = content.substring(startPos, endPos);
    int firstPipe = line.indexOf('|');
    if (firstPipe > 0) {
      String existingId = line.substring(0, firstPipe);
      if (existingId == id) {
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
      String existingId = line.substring(0, firstPipe);
      if (existingId == id) {
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
  file = storageFS->open(filename, "w");
  if (!file) {
    return false;
  }

  file.print(newContent);
  file.close();

  Serial.println("Quick script deleted from " + os + ": " + id);
  return true;
}

bool deleteAllQuickScripts(String os) {
  if (!storageAvailable || !storageFS) {
    Serial.println("Storage not available");
    return false;
  }

  String filename = getQuickScriptsFilename(os);

  if (storageFS->exists(filename)) {
    storageFS->remove(filename);
    Serial.println("All quick scripts deleted for: " + os);
    return true;
  }

  return false;
}

// File Management Functions

String sanitizeFilename(String filename) {
  // Ensure filename starts with /
  if (!filename.startsWith("/")) {
    filename = "/" + filename;
  }

  // Remove path traversal attempts
  filename.replace("../", "");
  filename.replace("..\\", "");

  // Replace null bytes and other dangerous characters
  filename.replace("\0", "");

  // Remove multiple consecutive slashes
  while (filename.indexOf("//") != -1) {
    filename.replace("//", "/");
  }

  // Don't allow just "/"
  if (filename == "/") {
    filename = "/file";
  }

  return filename;
}

bool hasAvailableSpace(size_t requiredBytes) {
  if (!storageAvailable || !storageFS) {
    return false;
  }

  size_t totalBytes = 0;
  size_t usedBytes = 0;
  getFilesystemInfo(totalBytes, usedBytes);

  if (totalBytes == 0) {
    return false;
  }

  size_t availableBytes = totalBytes - usedBytes;

  // Keep 10% safety margin
  size_t safetyMargin = totalBytes / 10;

  return (availableBytes - safetyMargin) >= requiredBytes;
}

bool getFilesystemInfo(size_t &totalBytes, size_t &usedBytes) {
  if (!storageAvailable) {
    return false;
  }

  if (usingSD) {
     totalBytes = SD_MMC.totalBytes();
     usedBytes = SD_MMC.usedBytes();
  } else {
     totalBytes = LittleFS.totalBytes();
     usedBytes = LittleFS.usedBytes();
  }

  if (totalBytes == 0) {
    return false;
  }

  return true;
}