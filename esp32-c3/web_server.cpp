#include "web_server.h"
#include <WebServer.h>
#include <LittleFS.h>
#include "wifi_manager.h"
#include "display_manager.h"
#include "hid_handler.h"
#include "ducky_parser.h"
#include "littlefs_manager.h"
#include "utils.h"
#include "config.h"

#if ENABLE_HTTPS
#include <WebServerSecure.h>
#include "certs.h"
#endif

WebServer server(80);

#if ENABLE_HTTPS
WebServerSecure secureServer(443);
bool httpsEnabled = false;
#endif

// Macro to handle both HTTP and HTTPS server responses
#if ENABLE_HTTPS
#define GET_ACTIVE_SERVER() (httpsEnabled && secureServer.client() ? secureServer : server)
#define SERVER_SEND(code, type, content) \
  do { \
    if (httpsEnabled && secureServer.client()) { \
      secureServer.send(code, type, content); \
    } else { \
      server.send(code, type, content); \
    } \
  } while(0)

#define SERVER_HAS_ARG(argname) (httpsEnabled && secureServer.client() ? secureServer.hasArg(argname) : server.hasArg(argname))
#define SERVER_ARG(argname) (httpsEnabled && secureServer.client() ? secureServer.arg(argname) : server.arg(argname))
#define SERVER_STREAM_FILE(file, type) \
  do { \
    if (httpsEnabled && secureServer.client()) { \
      secureServer.streamFile(file, type); \
    } else { \
      server.streamFile(file, type); \
    } \
  } while(0)
#define SERVER_AUTHENTICATE(user, pass) (httpsEnabled && secureServer.client() ? secureServer.authenticate(user, pass) : server.authenticate(user, pass))
#define SERVER_REQUEST_AUTH() \
  do { \
    if (httpsEnabled && secureServer.client()) { \
      secureServer.requestAuthentication(); \
    } else { \
      server.requestAuthentication(); \
    } \
  } while(0)
#else
#define SERVER_SEND(code, type, content) server.send(code, type, content)
#define SERVER_HAS_ARG(argname) server.hasArg(argname)
#define SERVER_ARG(argname) server.arg(argname)
#define SERVER_STREAM_FILE(file, type) server.streamFile(file, type)
#define SERVER_AUTHENTICATE(user, pass) server.authenticate(user, pass)
#define SERVER_REQUEST_AUTH() server.requestAuthentication()
#endif

void serveStaticFile(String path, String contentType) {
  if (littlefsAvailable) {
    File file = LittleFS.open(path, "r");
    if (file) {
      SERVER_STREAM_FILE(file, contentType);
      file.close();
      return;
    }
  }
  SERVER_SEND(404, "text/plain", "File not found");
}

bool checkAuthentication() {
  if (!SERVER_AUTHENTICATE(WEB_AUTH_USER, WEB_AUTH_PASS)) {
    SERVER_REQUEST_AUTH();
    return false;
  }
  return true;
}

void setupWebServer() {
  // Register routes on HTTP server
  server.on("/", HTTP_GET, handleRoot);
  server.on("/setup", HTTP_GET, handleSetup);
  server.on("/manage-actions.html", HTTP_GET, handleManageActions);
  server.on("/manage-os.html", HTTP_GET, handleManageOS);
  server.on("/manage-scripts.html", HTTP_GET, handleManageScripts);
  server.on("/trackpad-fullscreen.html", HTTP_GET, handleTrackpadFullscreen);
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
  server.on("/api/quickactions", HTTP_GET, handleListQuickActions);
  server.on("/api/quickactions", HTTP_POST, handleSaveQuickAction);
  server.on("/api/quickactions/delete", HTTP_POST, handleDeleteQuickAction);
  server.on("/api/quickactions/reorder", HTTP_POST, handleReorderQuickActions);
  server.on("/api/quickscripts", HTTP_GET, handleListQuickScripts);
  server.on("/api/quickscripts", HTTP_POST, handleSaveQuickScript);
  server.on("/api/quickscripts/delete", HTTP_POST, handleDeleteQuickScript);
  server.on("/api/customos", HTTP_GET, handleListCustomOS);
  server.on("/api/customos", HTTP_POST, handleSaveCustomOS);
  server.on("/api/customos/delete", HTTP_POST, handleDeleteCustomOS);
  server.on("/manage-files.html", HTTP_GET, handleManageFiles);
  server.on("/api/files", HTTP_GET, handleListFiles);
  server.on("/api/files/upload", HTTP_POST, handleFileUploadDone, handleFileUpload);
  server.on("/api/files/delete", HTTP_POST, handleFileDelete);
  server.on("/api/files/download", HTTP_GET, handleFileDownload);

  server.begin();
  Serial.println("HTTP server started on port 80");

#if ENABLE_HTTPS
  // Try to initialize HTTPS server
  Serial.println("Initializing HTTPS server...");

  // Set up the certificate and private key using BearSSL
  static BearSSL::X509List serverCert(server_cert, server_cert_len);
  static BearSSL::PrivateKey serverKey(server_key, server_key_len);

  secureServer.getServer().setRSACert(&serverCert, &serverKey);

  // Register same routes on HTTPS server
  secureServer.on("/", HTTP_GET, handleRoot);
  secureServer.on("/setup", HTTP_GET, handleSetup);
  secureServer.on("/manage-actions.html", HTTP_GET, handleManageActions);
  secureServer.on("/manage-os.html", HTTP_GET, handleManageOS);
  secureServer.on("/manage-scripts.html", HTTP_GET, handleManageScripts);
  secureServer.on("/trackpad-fullscreen.html", HTTP_GET, handleTrackpadFullscreen);
  secureServer.on("/style.css", HTTP_GET, handleCSS);
  secureServer.on("/script.js", HTTP_GET, handleJS);
  secureServer.on("/api/command", HTTP_POST, handleCommand);
  secureServer.on("/api/script", HTTP_POST, handleScript);
  secureServer.on("/api/jiggler", HTTP_GET, handleJiggler);
  secureServer.on("/api/status", HTTP_GET, handleStatus);
  secureServer.on("/api/wifi", HTTP_GET, handleGetWiFi);
  secureServer.on("/api/wifi", HTTP_POST, handleSetWiFi);
  secureServer.on("/api/scan", HTTP_GET, handleScan);
  secureServer.on("/api/scripts", HTTP_GET, handleListScripts);
  secureServer.on("/api/scripts", HTTP_POST, handleSaveScript);
  secureServer.on("/api/scripts/load", HTTP_POST, handleLoadScript);
  secureServer.on("/api/scripts/delete", HTTP_POST, handleDeleteScript);
  secureServer.on("/api/quickactions", HTTP_GET, handleListQuickActions);
  secureServer.on("/api/quickactions", HTTP_POST, handleSaveQuickAction);
  secureServer.on("/api/quickactions/delete", HTTP_POST, handleDeleteQuickAction);
  secureServer.on("/api/quickactions/reorder", HTTP_POST, handleReorderQuickActions);
  secureServer.on("/api/quickscripts", HTTP_GET, handleListQuickScripts);
  secureServer.on("/api/quickscripts", HTTP_POST, handleSaveQuickScript);
  secureServer.on("/api/quickscripts/delete", HTTP_POST, handleDeleteQuickScript);
  secureServer.on("/api/customos", HTTP_GET, handleListCustomOS);
  secureServer.on("/api/customos", HTTP_POST, handleSaveCustomOS);
  secureServer.on("/api/customos/delete", HTTP_POST, handleDeleteCustomOS);
  secureServer.on("/manage-files.html", HTTP_GET, handleManageFiles);
  secureServer.on("/api/files", HTTP_GET, handleListFiles);
  secureServer.on("/api/files/upload", HTTP_POST, handleFileUploadDone, handleFileUpload);
  secureServer.on("/api/files/delete", HTTP_POST, handleFileDelete);
  secureServer.on("/api/files/download", HTTP_GET, handleFileDownload);

  secureServer.begin();
  httpsEnabled = true;
  Serial.println("HTTPS server started on port 443");
  Serial.println("WARNING: HTTPS uses ~15-20KB RAM. Monitor free heap!");
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());
#else
  Serial.println("HTTPS is disabled (set ENABLE_HTTPS=1 in config.h to enable)");
#endif
}

void handleWebClients() {
  server.handleClient();
#if ENABLE_HTTPS
  if (httpsEnabled) {
    secureServer.handleClient();
  }
#endif
}

void handleRoot() {
  if (!checkAuthentication()) return;
  serveStaticFile("/index.html", "text/html");
}

void handleSetup() {
  if (!checkAuthentication()) return;
  serveStaticFile("/setup.html", "text/html");
}

void handleManageActions() {
  if (!checkAuthentication()) return;
  serveStaticFile("/manage-actions.html", "text/html");
}

void handleManageOS() {
  if (!checkAuthentication()) return;
  serveStaticFile("/manage-os.html", "text/html");
}

void handleCSS() {
  if (!checkAuthentication()) return;
  serveStaticFile("/style.css", "text/css");
}

void handleJS() {
  if (!checkAuthentication()) return;
  serveStaticFile("/script.js", "application/javascript");
}

void handleCommand() {
  if (!checkAuthentication()) return;
  if (SERVER_HAS_ARG("cmd")) {
    String cmd = SERVER_ARG("cmd");
    processHIDCommand(cmd);
    SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"message\":\"Command sent\"}");
  } else {
    SERVER_SEND(400, "application/json", "{status:error,message:Missing cmd parameter}");
  }
}

void handleScript() {
  if (!checkAuthentication()) return;
  if (SERVER_HAS_ARG("script")) {
    String script = SERVER_ARG("script");
    executeDuckyScript(script);
    displayAction("Script executed");
    SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"message\":\"Script executed\"}");
  } else {
    SERVER_SEND(400, "application/json", "{status:error,message:Missing script parameter}");
  }
}

void handleJiggler() {
  if (!checkAuthentication()) return;
  if (SERVER_HAS_ARG("enable")) {
    String enable = SERVER_ARG("enable");
    if (enable == "1") {
      // Get jiggler parameters (with defaults if not provided)
      String type = SERVER_HAS_ARG("type") ? SERVER_ARG("type") : "simple";
      String diameter = SERVER_HAS_ARG("diameter") ? SERVER_ARG("diameter") : "2";
      String delay = SERVER_HAS_ARG("delay") ? SERVER_ARG("delay") : "2000";

      // Send command with all parameters: JIGGLE_ON <type> <diameter> <delay>
      String command = "JIGGLE_ON " + type + " " + diameter + " " + delay;
      processHIDCommand(command);
      displayAction("Jiggler ON");
      SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"enabled\":true}");
    } else {
      processHIDCommand("JIGGLE_OFF");
      displayAction("Jiggler OFF");
      SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"enabled\":false}");
    }
  } else {
    SERVER_SEND(400, "application/json", "{status:error,message:Missing enable parameter}");
  }
}

void handleStatus() {
  if (!checkAuthentication()) return;
  String json = "{";
  json += "\"wifi_mode\":\"" + String(isAPMode ? "AP" : "Station") + "\",";
  json += "\"ssid\":\"" + (isAPMode ? String(AP_SSID) : currentSSID) + "\",";
  json += "\"ip\":\"" + (isAPMode ? "192.168.4.1" : WiFi.localIP().toString()) + "\",";
  json += "\"connected\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false");
  json += "}";
  SERVER_SEND(200, "application/json", json);
}

void handleGetWiFi() {
  if (!checkAuthentication()) return;
  String json = "{";
  json += "\"ssid\":\"" + currentSSID + "\",";
  json += "\"mode\":\"" + String(isAPMode ? "AP" : "Station") + "\"";
  json += "}";
  SERVER_SEND(200, "application/json", json);
}

void handleSetWiFi() {
  if (!checkAuthentication()) return;
  if (SERVER_HAS_ARG("ssid") && SERVER_HAS_ARG("password")) {
    String ssid = SERVER_ARG("ssid");
    String password = SERVER_ARG("password");

    saveWiFiCredentials(ssid, password);
    displayAction("WiFi saved");

    String json = "{\"status\":\"ok\",\"message\":\"WiFi credentials saved. Restarting...\"}";
    SERVER_SEND(200, "application/json", json);

    delay(1000);
    ESP.restart();
  } else {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing parameters\"}");
  }
}

void handleScan() {
  if (!checkAuthentication()) return;
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
  SERVER_SEND(200, "application/json", json);
}

// Script API Handlers
void handleListScripts() {
  if (!checkAuthentication()) return;
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
  SERVER_SEND(200, "application/json", json);
}

void handleSaveScript() {
  if (!checkAuthentication()) return;
  if (SERVER_HAS_ARG("name") && SERVER_HAS_ARG("script")) {
    String name = SERVER_ARG("name");
    String script = SERVER_ARG("script");

    if (name.length() == 0) {
      SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Script name cannot be empty\"}");
      return;
    }

    if (name.length() > MAX_SCRIPT_NAME_LEN) {
      SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Script name too long\"}");
      return;
    }

    if (saveScriptToFile(name, script)) {
      displayAction("Saved: " + name);
      SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"message\":\"Script saved\"}");
    } else {
      SERVER_SEND(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save script\"}");
    }
  } else {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing name or script parameter\"}");
  }
}

void handleLoadScript() {
  if (!checkAuthentication()) return;
  if (SERVER_HAS_ARG("name")) {
    String name = SERVER_ARG("name");
    String script = loadScriptFromFile(name);

    if (script.length() == 0) {
      SERVER_SEND(404, "application/json", "{\"status\":\"error\",\"message\":\"Script not found\"}");
      return;
    }

    displayAction("Loaded: " + name);

    String json = "{";
    json += "\"status\":\"ok\",";
    json += "\"name\":\"" + escapeJson(name) + "\",";
    json += "\"script\":\"" + escapeJson(script) + "\"";
    json += "}";

    SERVER_SEND(200, "application/json", json);
  } else {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing name parameter\"}");
  }
}

void handleDeleteScript() {
  if (!checkAuthentication()) return;
  if (SERVER_HAS_ARG("name")) {
    String name = SERVER_ARG("name");

    if (deleteScriptFile(name)) {
      displayAction("Deleted: " + name);
      SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"message\":\"Script deleted\"}");
    } else {
      SERVER_SEND(404, "application/json", "{\"status\":\"error\",\"message\":\"Script not found\"}");
    }
  } else {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing name parameter\"}");
  }
}

// Quick Actions Management Handlers

void handleListQuickActions() {
  if (!checkAuthentication()) return;

  if (!SERVER_HAS_ARG("os")) {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing os parameter\"}");
    return;
  }

  String os = SERVER_ARG("os");
  String content = loadQuickActions(os);

  String json = "[";
  if (content.length() > 0) {
    bool first = true;
    int startPos = 0;
    int endPos;

    while ((endPos = content.indexOf('\n', startPos)) != -1) {
      String line = content.substring(startPos, endPos);
      if (line.length() > 0) {
        // Parse line: cmd|label|desc|class
        int pipe1 = line.indexOf('|');
        int pipe2 = line.indexOf('|', pipe1 + 1);
        int pipe3 = line.indexOf('|', pipe2 + 1);

        if (pipe1 > 0 && pipe2 > pipe1 && pipe3 > pipe2) {
          String cmd = line.substring(0, pipe1);
          String label = line.substring(pipe1 + 1, pipe2);
          String desc = line.substring(pipe2 + 1, pipe3);
          String btnClass = line.substring(pipe3 + 1);

          if (!first) json += ",";
          first = false;

          json += "{";
          json += "\"cmd\":\"" + escapeJson(cmd) + "\",";
          json += "\"label\":\"" + escapeJson(label) + "\",";
          json += "\"desc\":\"" + escapeJson(desc) + "\",";
          json += "\"class\":\"" + escapeJson(btnClass) + "\"";
          json += "}";
        }
      }
      startPos = endPos + 1;
    }
  }

  json += "]";
  SERVER_SEND(200, "application/json", json);
}

void handleSaveQuickAction() {
  if (!checkAuthentication()) return;

  if (!SERVER_HAS_ARG("os") || !SERVER_HAS_ARG("cmd") || !SERVER_HAS_ARG("label") ||
      !SERVER_HAS_ARG("desc") || !SERVER_HAS_ARG("class")) {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing required parameters\"}");
    return;
  }

  String os = SERVER_ARG("os");
  String cmd = SERVER_ARG("cmd");
  String label = SERVER_ARG("label");
  String desc = SERVER_ARG("desc");
  String btnClass = SERVER_ARG("class");

  if (cmd.length() == 0 || label.length() == 0) {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Command and label cannot be empty\"}");
    return;
  }

  if (saveQuickAction(os, cmd, label, desc, btnClass)) {
    displayAction("Quick action saved");
    SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"message\":\"Quick action saved\"}");
  } else {
    SERVER_SEND(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save quick action\"}");
  }
}

void handleDeleteQuickAction() {
  if (!checkAuthentication()) return;

  if (!SERVER_HAS_ARG("os") || !SERVER_HAS_ARG("cmd")) {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing os or cmd parameter\"}");
    return;
  }

  String os = SERVER_ARG("os");
  String cmd = SERVER_ARG("cmd");

  if (deleteQuickAction(os, cmd)) {
    displayAction("Quick action deleted");
    SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"message\":\"Quick action deleted\"}");
  } else {
    SERVER_SEND(404, "application/json", "{\"status\":\"error\",\"message\":\"Quick action not found\"}");
  }
}

// Custom OS Management Handlers

void handleListCustomOS() {
  if (!checkAuthentication()) return;

  String content = loadCustomOSList();

  String json = "[";
  if (content.length() > 0) {
    bool first = true;
    int startPos = 0;
    int endPos;

    while ((endPos = content.indexOf('\n', startPos)) != -1) {
      String line = content.substring(startPos, endPos);
      if (line.length() > 0) {
        if (!first) json += ",";
        first = false;
        json += "\"" + escapeJson(line) + "\"";
      }
      startPos = endPos + 1;
    }
  }

  json += "]";
  SERVER_SEND(200, "application/json", json);
}

void handleSaveCustomOS() {
  if (!checkAuthentication()) return;

  if (!SERVER_HAS_ARG("name")) {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing name parameter\"}");
    return;
  }

  String osName = SERVER_ARG("name");

  if (osName.length() == 0) {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"OS name cannot be empty\"}");
    return;
  }

  // Check if it's a default OS
  if (osName == "Windows" || osName == "MacOS" || osName == "Linux") {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Cannot add default OS as custom\"}");
    return;
  }

  if (addCustomOS(osName)) {
    displayAction("Custom OS added: " + osName);
    SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"message\":\"Custom OS added\"}");
  } else {
    SERVER_SEND(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to add custom OS\"}");
  }
}

void handleDeleteCustomOS() {
  if (!checkAuthentication()) return;

  if (!SERVER_HAS_ARG("name")) {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing name parameter\"}");
    return;
  }

  String osName = SERVER_ARG("name");

  if (deleteCustomOS(osName)) {
    displayAction("Custom OS deleted: " + osName);
    SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"message\":\"Custom OS deleted\"}");
  } else {
    SERVER_SEND(404, "application/json", "{\"status\":\"error\",\"message\":\"Custom OS not found\"}");
  }
}

// Quick Actions Reordering Handler

void handleReorderQuickActions() {
  if (!checkAuthentication()) return;

  if (!SERVER_HAS_ARG("os") || !SERVER_HAS_ARG("order")) {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing os or order parameter\"}");
    return;
  }

  String os = SERVER_ARG("os");
  String order = SERVER_ARG("order"); // Comma-separated list of cmd values

  // Load existing actions
  String content = loadQuickActions(os);
  if (content.length() == 0) {
    SERVER_SEND(404, "application/json", "{\"status\":\"error\",\"message\":\"No actions found for this OS\"}");
    return;
  }

  // Parse order list
  String newContent = "";
  int orderStart = 0;
  int orderEnd;

  while ((orderEnd = order.indexOf(',', orderStart)) != -1) {
    String cmd = order.substring(orderStart, orderEnd);
    cmd.trim();

    // Find this action in content and add it
    int startPos = 0;
    int endPos;
    while ((endPos = content.indexOf('\n', startPos)) != -1) {
      String line = content.substring(startPos, endPos);
      if (line.startsWith(cmd + "|")) {
        newContent += line + "\n";
        break;
      }
      startPos = endPos + 1;
    }

    orderStart = orderEnd + 1;
  }

  // Handle last cmd in order
  if (orderStart < order.length()) {
    String cmd = order.substring(orderStart);
    cmd.trim();

    int startPos = 0;
    int endPos;
    while ((endPos = content.indexOf('\n', startPos)) != -1) {
      String line = content.substring(startPos, endPos);
      if (line.startsWith(cmd + "|")) {
        newContent += line + "\n";
        break;
      }
      startPos = endPos + 1;
    }
  }

  // Save reordered content
  String filename = String("/quickactions_") + os + ".txt";
  filename.replace(" ", "_");

  File file = LittleFS.open(filename, "w");
  if (!file) {
    SERVER_SEND(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save reordered actions\"}");
    return;
  }

  file.print(newContent);
  file.close();

  displayAction("Quick actions reordered");
  SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"message\":\"Actions reordered\"}");
}

// Quick Scripts Management Handlers

void handleManageScripts() {
  if (!checkAuthentication()) return;
  serveStaticFile("/manage-scripts.html", "text/html");
}

void handleTrackpadFullscreen() {
  if (!checkAuthentication()) return;
  serveStaticFile("/trackpad-fullscreen.html", "text/html");
}

void handleListQuickScripts() {
  if (!checkAuthentication()) return;

  if (!SERVER_HAS_ARG("os")) {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing os parameter\"}");
    return;
  }

  String os = SERVER_ARG("os");
  String content = loadQuickScripts(os);

  String json = "[";
  if (content.length() > 0) {
    bool first = true;
    int startPos = 0;
    int endPos;

    while ((endPos = content.indexOf('\n', startPos)) != -1) {
      String line = content.substring(startPos, endPos);
      if (line.length() > 0) {
        // Parse line: id|label|script|class
        int pipe1 = line.indexOf('|');
        int pipe2 = line.indexOf('|', pipe1 + 1);
        int pipe3 = line.indexOf('|', pipe2 + 1);

        if (pipe1 > 0 && pipe2 > pipe1 && pipe3 > pipe2) {
          String id = line.substring(0, pipe1);
          String label = line.substring(pipe1 + 1, pipe2);
          String script = line.substring(pipe2 + 1, pipe3);
          String btnClass = line.substring(pipe3 + 1);

          // Unescape newlines
          script.replace("\\n", "\n");

          if (!first) json += ",";
          first = false;

          json += "{";
          json += "\"id\":\"" + escapeJson(id) + "\",";
          json += "\"label\":\"" + escapeJson(label) + "\",";
          json += "\"script\":\"" + escapeJson(script) + "\",";
          json += "\"class\":\"" + escapeJson(btnClass) + "\"";
          json += "}";
        }
      }
      startPos = endPos + 1;
    }
  }

  json += "]";
  SERVER_SEND(200, "application/json", json);
}

void handleSaveQuickScript() {
  if (!checkAuthentication()) return;

  if (!SERVER_HAS_ARG("os") || !SERVER_HAS_ARG("id") || !SERVER_HAS_ARG("label") ||
      !SERVER_HAS_ARG("script") || !SERVER_HAS_ARG("class")) {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing required parameters\"}");
    return;
  }

  String os = SERVER_ARG("os");
  String id = SERVER_ARG("id");
  String label = SERVER_ARG("label");
  String script = SERVER_ARG("script");
  String btnClass = SERVER_ARG("class");

  if (id.length() == 0 || label.length() == 0 || script.length() == 0) {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"ID, label, and script cannot be empty\"}");
    return;
  }

  if (saveQuickScript(os, id, label, script, btnClass)) {
    displayAction("Quick script saved");
    SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"message\":\"Quick script saved\"}");
  } else {
    SERVER_SEND(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save quick script\"}");
  }
}

void handleDeleteQuickScript() {
  if (!checkAuthentication()) return;

  if (!SERVER_HAS_ARG("os") || !SERVER_HAS_ARG("id")) {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing os or id parameter\"}");
    return;
  }

  String os = SERVER_ARG("os");
  String id = SERVER_ARG("id");

  if (deleteQuickScript(os, id)) {
    displayAction("Quick script deleted");
    SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"message\":\"Quick script deleted\"}");
  } else {
    SERVER_SEND(404, "application/json", "{\"status\":\"error\",\"message\":\"Quick script not found\"}");
  }
}

// File Management Handlers

void handleManageFiles() {
  if (!checkAuthentication()) return;
  serveStaticFile("/manage-files.html", "text/html");
}

void handleListFiles() {
  if (!checkAuthentication()) return;

  if (!littlefsAvailable) {
    SERVER_SEND(503, "application/json", "{\"status\":\"error\",\"message\":\"LittleFS not available\"}");
    return;
  }

  size_t totalBytes = 0;
  size_t usedBytes = 0;

  if (!getFilesystemInfo(totalBytes, usedBytes)) {
    SERVER_SEND(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to get filesystem info\"}");
    return;
  }

  String json = "{";
  json += "\"filesystem\":{";
  json += "\"total\":" + String(totalBytes) + ",";
  json += "\"used\":" + String(usedBytes) + ",";
  json += "\"free\":" + String(totalBytes - usedBytes);
  json += "},";
  json += "\"files\":[";

  Dir dir = LittleFS.openDir("/");
  bool first = true;

  while (dir.next()) {
    if (!first) json += ",";
    first = false;

    String filename = dir.fileName();
    size_t filesize = dir.fileSize();

    json += "{";
    json += "\"name\":\"" + escapeJson(filename) + "\",";
    json += "\"size\":" + String(filesize);
    json += "}";
  }

  json += "]}";
  SERVER_SEND(200, "application/json", json);
}

// Global variable for file upload
File uploadFile;

void handleFileUpload() {
  if (!checkAuthentication()) return;

  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (filename.length() == 0) {
      Serial.println("Upload error: No filename");
      return;
    }

    // Sanitize filename
    filename = sanitizeFilename(filename);
    Serial.println("Upload start: " + filename);

    // Check available space (rough estimate)
    if (!hasAvailableSpace(100000)) { // Reserve 100KB minimum
      Serial.println("Upload error: Insufficient space");
      return;
    }

    // Open file for writing
    uploadFile = LittleFS.open(filename, "w");
    if (!uploadFile) {
      Serial.println("Upload error: Failed to open file for writing");
    }
  }
  else if (upload.status == UPLOAD_FILE_WRITE) {
    // Write chunk to file
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
      Serial.print(".");
    }
  }
  else if (upload.status == UPLOAD_FILE_END) {
    // Close file
    if (uploadFile) {
      uploadFile.close();
      Serial.println("\nUpload complete: " + String(upload.totalSize) + " bytes");
      displayAction("File uploaded: " + upload.filename);
    }
  }
}

void handleFileUploadDone() {
  if (!checkAuthentication()) return;

  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_END) {
    SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"message\":\"File uploaded successfully\"}");
  } else {
    SERVER_SEND(500, "application/json", "{\"status\":\"error\",\"message\":\"Upload failed\"}");
  }
}

void handleFileDelete() {
  if (!checkAuthentication()) return;

  if (!littlefsAvailable) {
    SERVER_SEND(503, "application/json", "{\"status\":\"error\",\"message\":\"LittleFS not available\"}");
    return;
  }

  if (!SERVER_HAS_ARG("name")) {
    SERVER_SEND(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing name parameter\"}");
    return;
  }

  String filename = SERVER_ARG("name");

  // Ensure filename starts with /
  if (!filename.startsWith("/")) {
    filename = "/" + filename;
  }

  if (!LittleFS.exists(filename)) {
    SERVER_SEND(404, "application/json", "{\"status\":\"error\",\"message\":\"File not found\"}");
    return;
  }

  if (LittleFS.remove(filename)) {
    Serial.println("File deleted: " + filename);
    displayAction("File deleted: " + filename);
    SERVER_SEND(200, "application/json", "{\"status\":\"ok\",\"message\":\"File deleted\"}");
  } else {
    SERVER_SEND(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to delete file\"}");
  }
}

void handleFileDownload() {
  if (!checkAuthentication()) return;

  if (!littlefsAvailable) {
    SERVER_SEND(503, "text/plain", "LittleFS not available");
    return;
  }

  if (!SERVER_HAS_ARG("name")) {
    SERVER_SEND(400, "text/plain", "Missing name parameter");
    return;
  }

  String filename = SERVER_ARG("name");

  // Ensure filename starts with /
  if (!filename.startsWith("/")) {
    filename = "/" + filename;
  }

  if (!LittleFS.exists(filename)) {
    SERVER_SEND(404, "text/plain", "File not found");
    return;
  }

  File file = LittleFS.open(filename, "r");
  if (!file) {
    SERVER_SEND(500, "text/plain", "Failed to open file");
    return;
  }

  // Get clean filename (without path) for Content-Disposition
  String cleanFilename = filename;
  int lastSlash = filename.lastIndexOf('/');
  if (lastSlash >= 0) {
    cleanFilename = filename.substring(lastSlash + 1);
  }

  // Set Content-Disposition header for download
  String contentDisposition = "attachment; filename=\"" + cleanFilename + "\"";

#if ENABLE_HTTPS
  if (httpsEnabled && secureServer.client()) {
    secureServer.sendHeader("Content-Disposition", contentDisposition);
    secureServer.streamFile(file, "application/octet-stream");
  } else {
    server.sendHeader("Content-Disposition", contentDisposition);
    server.streamFile(file, "application/octet-stream");
  }
#else
  server.sendHeader("Content-Disposition", contentDisposition);
  server.streamFile(file, "application/octet-stream");
#endif

  file.close();
  Serial.println("File downloaded: " + filename);
}