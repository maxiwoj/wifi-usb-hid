/*
 * WiFi USB HID Control - NodeMCU
 *
 * This sketch runs on NodeMCU (ESP8266)
 * Provides WiFi connectivity and web interface for controlling Pro Micro
 *
 * Features:
 * - WiFi Manager with EEPROM storage
 * - Web-based control interface
 * - DuckyScript parser
 * - Network scanner
 * - REST API endpoints
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <LittleFS.h>

// EEPROM addresses (for WiFi credentials only)
#define EEPROM_SIZE 512
#define SSID_ADDR 0
#define PASS_ADDR 100

// LittleFS settings for script storage
#define MAX_SCRIPT_NAME_LEN 32

// WiFi AP settings
#define AP_SSID "USB-HID-Setup"
#define AP_PASS "12345678"

// WiFi connection timeout
#define WIFI_TIMEOUT 10000

// Web server
ESP8266WebServer server(80);

// State variables
String currentSSID = "";
String currentPassword = "";
bool isAPMode = false;
bool littlefsAvailable = false;

void setup() {
  Serial.println("Device is booting up...");
  // Initialize serial communication
  Serial.begin(74880);
  delay(100);

  // Initialize EEPROM (for WiFi credentials)
  EEPROM.begin(EEPROM_SIZE);

  // Initialize LittleFS (for script storage)
  // Note: LittleFS must be uploaded via Tools > ESP8266 Sketch Data Upload in Arduino IDE
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

  // Try to connect to saved WiFi
  loadWiFiCredentials();
 
  if (currentSSID.length() > 0) {
    Serial.println("Attempting to connect to saved network: " + currentSSID);
    if (!connectToWiFi(currentSSID, currentPassword)) {
      Serial.println("Failed to connect, starting AP mode");
      startAPMode();
    }
  } else {
    Serial.println("No saved credentials, starting AP mode");
    startAPMode();
  }

  // Setup web server routes
  setupWebServer();

  // Start web server
  server.begin();
  Serial.println("Web server started");

  if (isAPMode) {
    Serial.println("AP Mode - IP: 192.168.4.1");
  } else {
    Serial.println("Station Mode - IP: " + WiFi.localIP().toString());
  }
}

void loop() {
  server.handleClient();
}

// WiFi Management Functions
void loadWiFiCredentials() {
  currentSSID = readStringFromEEPROM(SSID_ADDR, 32);
  currentPassword = readStringFromEEPROM(PASS_ADDR, 64);
}

void saveWiFiCredentials(String ssid, String password) {
  writeStringToEEPROM(SSID_ADDR, ssid, 32);
  writeStringToEEPROM(PASS_ADDR, password, 64);
  EEPROM.commit();
}

bool connectToWiFi(String ssid, String password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startTime > WIFI_TIMEOUT) {
      return false;
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.println("IP: " + WiFi.localIP().toString());
  isAPMode = false;
  return true;
}

void startAPMode() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.println("AP Mode started");
  Serial.println("SSID: " + String(AP_SSID));
  Serial.println("Password: " + String(AP_PASS));
  Serial.println("IP: 192.168.4.1");
  isAPMode = true;
}

// EEPROM Helper Functions
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

// Forward declarations
String escapeJson(String str);

// Web Server Setup
void setupWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/setup", HTTP_GET, handleSetup);
  server.on("/style.css", HTTP_GET, handleCSS);
  server.on("/script.js", HTTP_GET, handleJS);
  server.on("/api/command", HTTP_POST, handleCommand);
  server.on("/api/script", HTTP_POST, handleScript);
  server.on("/api/jiggler", HTTP_GET, handleJiggler);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/wifi", HTTP_GET, handleGetWiFi);
  server.on("/api/wifi", HTTP_POST, handleSetWiFi);
  server.on("/api/scan", HTTP_GET, handleScan);
  server.on("/api/scripts", HTTP_GET, handleListScripts);
  server.on("/api/scripts", HTTP_POST, handleSaveScript);
  server.on("/api/scripts/load", HTTP_POST, handleLoadScript);
  server.on("/api/scripts/delete", HTTP_POST, handleDeleteScript);
}

void serveStaticFile(String path, String contentType) {
  if (littlefsAvailable) {
    File file = LittleFS.open(path, "r");
    if (file) {
      server.streamFile(file, contentType);
      file.close();
      return;
    }
  }
  server.send(404, "text/plain", "File not found");
}

// Web Server Handlers
void handleRoot() {
  serveStaticFile("/index.html", "text/html");
}

void handleSetup() {
  serveStaticFile("/setup.html", "text/html");
}

void handleCSS() {
  serveStaticFile("/style.css", "text/css");
}

void handleJS() {
  serveStaticFile("/script.js", "application/javascript");
}

void handleCommand() {
  if (server.hasArg("cmd")) {
    String cmd = server.arg("cmd");
    sendCommandToProMicro(cmd);
    server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Command sent\"}");
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing cmd parameter\"}");
  }
}

void handleScript() {
  if (server.hasArg("script")) {
    String script = server.arg("script");
    executeDuckyScript(script);
    server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Script executed\"}");
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing script parameter\"}");
  }
}

void handleJiggler() {
  if (server.hasArg("enable")) {
    String enable = server.arg("enable");
    if (enable == "1") {
      sendCommandToProMicro("JIGGLE_ON");
      server.send(200, "application/json", "{\"status\":\"ok\",\"enabled\":true}");
    } else {
      sendCommandToProMicro("JIGGLE_OFF");
      server.send(200, "application/json", "{\"status\":\"ok\",\"enabled\":false}");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing enable parameter\"}");
  }
}

void handleStatus() {
  String json = "{";
  json += "\"wifi_mode\":\"" + String(isAPMode ? "AP" : "Station") + "\",";
  json += "\"ssid\":\"" + (isAPMode ? String(AP_SSID) : currentSSID) + "\",";
  json += "\"ip\":\"" + (isAPMode ? "192.168.4.1" : WiFi.localIP().toString()) + "\",";
  json += "\"connected\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

void handleGetWiFi() {
  String json = "{";
  json += "\"ssid\":\"" + currentSSID + "\",";
  json += "\"mode\":\"" + String(isAPMode ? "AP" : "Station") + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void handleSetWiFi() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    saveWiFiCredentials(ssid, password);

    String json = "{\"status\":\"ok\",\"message\":\"WiFi credentials saved. Restarting...\"}";
    server.send(200, "application/json", json);

    delay(1000);
    ESP.restart();
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing parameters\"}");
  }
}

void handleScan() {
  int n = WiFi.scanNetworks();
  String json = "[";

  for (int i = 0; i < n; i++) {
    if (i > 0) json += ",";
    json += "{";
    json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
    json += "\"encryption\":" + String(WiFi.encryptionType(i));
    json += "}";
  }

  json += "]";
  server.send(200, "application/json", json);
}

// Script Storage Functions (using LittleFS)
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

// Script API Handlers
void handleListScripts() {
  String json = "[";
  bool first = true;

  // Check if LittleFS is available
  if (littlefsAvailable) {
    // Iterate through all files in LittleFS
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
      String filename = dir.fileName();

      // Check if it's a script file (starts with "/scripts_")
      if (filename.startsWith("scripts_") && filename.endsWith(".txt")) {
        if (!first) json += ",";
        first = false;

        String name = getScriptNameFromFilename(filename);
        json += "{";
        json += "\"name\":\"" + escapeJson(name) + "\"";
        json += "}";
      }
    }
  }

  json += "]";
  server.send(200, "application/json", json);
}

void handleSaveScript() {
  if (server.hasArg("name") && server.hasArg("script")) {
    String name = server.arg("name");
    String script = server.arg("script");

    if (name.length() == 0) {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Script name cannot be empty\"}");
      return;
    }

    if (name.length() > MAX_SCRIPT_NAME_LEN) {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Script name too long\"}");
      return;
    }

    if (saveScriptToFile(name, script)) {
      server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Script saved\"}");
    } else {
      server.send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save script\"}");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing name or script parameter\"}");
  }
}

void handleLoadScript() {
  if (server.hasArg("name")) {
    String name = server.arg("name");
    String script = loadScriptFromFile(name);

    if (script.length() == 0) {
      server.send(404, "application/json", "{\"status\":\"error\",\"message\":\"Script not found\"}");
      return;
    }

    String json = "{";
    json += "\"status\":\"ok\",";
    json += "\"name\":\"" + escapeJson(name) + "\",";
    json += "\"script\":\"" + escapeJson(script) + "\"";
    json += "}";

    server.send(200, "application/json", json);
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing name parameter\"}");
  }
}

void handleDeleteScript() {
  if (server.hasArg("name")) {
    String name = server.arg("name");

    if (deleteScriptFile(name)) {
      server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Script deleted\"}");
    } else {
      server.send(404, "application/json", "{\"status\":\"error\",\"message\":\"Script not found\"}");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing name parameter\"}");
  }
}

String escapeJson(String str) {
  String result = "";
  for (int i = 0; i < str.length(); i++) {
    char c = str[i];
    if (c == '\\') {
      result += "\\\\";
    } else if (c == '"') {
      result += "\\\"";
    } else if (c == '\n') {
      result += "\\n";
    } else if (c == '\r') {
      result += "\\r";
    } else if (c == '\t') {
      result += "\\t";
    } else {
      result += c;
    }
  }
  return result;
}

// Communication with Pro Micro
void sendCommandToProMicro(String cmd) {
  Serial.print("Sending command to Pro Micro: ");
  Serial.println(cmd);
  Serial.flush();
}

// DuckyScript Parser
void executeDuckyScript(String script) {
  Serial.println("Executing Ducky Script...");
  int lineStart = 0;
  int lineEnd = 0;

  while (lineEnd != -1) {
    lineEnd = script.indexOf('\n', lineStart);
    String line;

    if (lineEnd == -1) {
      line = script.substring(lineStart);
    } else {
      line = script.substring(lineStart, lineEnd);
      lineStart = lineEnd + 1;
    }

    line.trim();

    // Skip empty lines and comments
    if (line.length() == 0 || line.startsWith("//")) {
      continue;
    }

    parseDuckyLine(line);

    if (lineEnd == -1) break;
  }
}

void parseDuckyLine(String line) {
  line.trim();

  if (line.startsWith("DELAY ")) {
    String delayMs = line.substring(6);
    sendCommandToProMicro("DELAY:" + delayMs);
  }
  else if (line.startsWith("STRING ")) {
    String text = line.substring(7);
    sendCommandToProMicro("TYPE:" + text);
  }
  else if (line == "ENTER") {
    sendCommandToProMicro("ENTER");
  }
  else if (line == "ESC") {
    sendCommandToProMicro("ESC");
  }
  else if (line == "TAB") {
    sendCommandToProMicro("TAB");
  }
  else if (line == "BACKSPACE") {
    sendCommandToProMicro("BACKSPACE");
  }
  else if (line == "DELETE") {
    sendCommandToProMicro("DELETE");
  }
  else if (line.startsWith("GUI ")) {
    String key = line.substring(4);
    if (key == "r" || key == "R") {
      sendCommandToProMicro("GUI_R");
    } else if (key == "d" || key == "D") {
      sendCommandToProMicro("GUI_D");
    } else if (key == "SPACE") {
      sendCommandToProMicro("GUI_SPACE");
    } else if (key == "TAB" || key == "tab") {
      sendCommandToProMicro("GUI_TAB");
    } else if (key == "h" || key == "H") {
      sendCommandToProMicro("GUI_H");
    } else if (key == "w" || key == "W") {
      sendCommandToProMicro("GUI_W");
    } else {
      sendCommandToProMicro("GUI_" + key);
    }
  }
  else if (line == "GUI") {
    sendCommandToProMicro("GUI");
  }
  else if (line == "ALT TAB") {
    sendCommandToProMicro("ALT_TAB");
  }
  else if (line == "CTRL ALT DELETE") {
    sendCommandToProMicro("CTRL_ALT_DEL");
  }
  else if (line == "CTRL ALT T") {
    sendCommandToProMicro("CTRL_ALT_T");
  }
  else if (line.startsWith("CTRL ")) {
    String key = line.substring(5);
    sendCommandToProMicro("CTRL_" + key);
  }
  else if (line.startsWith("ALT ")) {
    String key = line.substring(4);
    sendCommandToProMicro("ALT_" + key);
  }
  else if (line == "UP") {
    sendCommandToProMicro("UP");
  }
  else if (line == "DOWN") {
    sendCommandToProMicro("DOWN");
  }
  else if (line == "LEFT") {
    sendCommandToProMicro("LEFT");
  }
  else if (line == "RIGHT") {
    sendCommandToProMicro("RIGHT");
  }
  else if (line == "F1" || line == "F2" || line == "F3" || line == "F4" ||
           line == "F5" || line == "F6" || line == "F7" || line == "F8" ||
           line == "F9" || line == "F10" || line == "F11" || line == "F12") {
    sendCommandToProMicro(line);
  }
}

// Quick Scripts
String getQuickScript(String scriptName, String os) {
  if (os == "Windows") {
    if (scriptName == "editor") {
      return "GUI r\nDELAY 500\nSTRING notepad\nENTER";
    } else if (scriptName == "terminal") {
      return "GUI r\nDELAY 500\nSTRING cmd\nENTER";
    } else if (scriptName == "calculator") {
      return "GUI r\nDELAY 500\nSTRING calc\nENTER";
    } else if (scriptName == "browser") {
      return "GUI r\nDELAY 500\nSTRING chrome\nENTER";
    }
  } else if (os == "MacOS") {
    if (scriptName == "editor") {
      return "GUI SPACE\nDELAY 500\nSTRING textedit\nENTER";
    } else if (scriptName == "terminal") {
      return "GUI SPACE\nDELAY 500\nSTRING terminal\nENTER";
    } else if (scriptName == "calculator") {
      return "GUI SPACE\nDELAY 500\nSTRING calculator\nENTER";
    } else if (scriptName == "browser") {
      return "GUI SPACE\nDELAY 500\nSTRING safari\nENTER";
    }
  } else if (os == "Linux") {
    if (scriptName == "editor") {
      return "CTRL ALT T\nDELAY 1000\nSTRING gedit\nENTER";
    } else if (scriptName == "terminal") {
      return "CTRL ALT T";
    } else if (scriptName == "calculator") {
      return "GUI\nDELAY 500\nSTRING calc\nENTER";
    } else if (scriptName == "browser") {
      return "GUI\nDELAY 500\nSTRING firefox\nENTER";
    }
  }
  return "";
}
