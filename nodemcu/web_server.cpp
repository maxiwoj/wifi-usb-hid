#include "web_server.h"
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include "wifi_manager.h"
#include "display_manager.h"
#include "pro_micro.h"
#include "ducky_parser.h"
#include "littlefs_manager.h"
#include "utils.h"
#include "config.h"

#if ENABLE_HTTPS
#include <ESP8266WebServerSecure.h>
#include "certs.h"
#endif

ESP8266WebServer server(80);

#if ENABLE_HTTPS
ESP8266WebServerSecure secureServer(443);
bool httpsEnabled = false;
#endif

// Helper to get the current server (HTTP or HTTPS)
#if ENABLE_HTTPS
ESP8266WebServer* getCurrentServer() {
  // Check if request is from HTTPS server
  if (httpsEnabled && secureServer.client()) {
    return &secureServer;
  }
  return &server;
}
#else
ESP8266WebServer* getCurrentServer() {
  return &server;
}
#endif

void serveStaticFile(String path, String contentType) {
  ESP8266WebServer* activeServer = getCurrentServer();
  if (littlefsAvailable) {
    File file = LittleFS.open(path, "r");
    if (file) {
      activeServer->streamFile(file, contentType);
      file.close();
      return;
    }
  }
  activeServer->send(404, "text/plain", "File not found");
}

bool checkAuthentication() {
  ESP8266WebServer* activeServer = getCurrentServer();
  if (!activeServer->authenticate(WEB_AUTH_USER, WEB_AUTH_PASS)) {
    activeServer->requestAuthentication();
    return false;
  }
  return true;
}

void setupWebServer() {
  // Register routes on HTTP server
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

  server.begin();
  Serial.println("HTTP server started on port 80");

#if ENABLE_HTTPS
  // Try to initialize HTTPS server
  Serial.println("Initializing HTTPS server...");

  // Set up the certificate and private key
  secureServer.getServer().setRSACert(server_cert, server_cert_len, server_key, server_key_len);

  // Register same routes on HTTPS server
  secureServer.on("/", HTTP_GET, handleRoot);
  secureServer.on("/setup", HTTP_GET, handleSetup);
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
  ESP8266WebServer* activeServer = getCurrentServer();
  if (activeServer->hasArg("cmd")) {
    String cmd = activeServer->arg("cmd");
    sendCommandToProMicro(cmd);
    activeServer->send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Command sent\"}");
  } else {
    activeServer->send(400, "application/json", "{status:error,message:Missing cmd parameter}");
  }
}

void handleScript() {
  if (!checkAuthentication()) return;
  ESP8266WebServer* activeServer = getCurrentServer();
  if (activeServer->hasArg("script")) {
    String script = activeServer->arg("script");
    executeDuckyScript(script);
    displayAction("Script executed");
    activeServer->send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Script executed\"}");
  } else {
    activeServer->send(400, "application/json", "{status:error,message:Missing script parameter}");
  }
}

void handleJiggler() {
  if (!checkAuthentication()) return;
  ESP8266WebServer* activeServer = getCurrentServer();
  if (activeServer->hasArg("enable")) {
    String enable = activeServer->arg("enable");
    if (enable == "1") {
      sendCommandToProMicro("JIGGLE_ON");
      displayAction("Jiggler ON");
      activeServer->send(200, "application/json", "{\"status\":\"ok\",\"enabled\":true}");
    } else {
      sendCommandToProMicro("JIGGLE_OFF");
      displayAction("Jiggler OFF");
      activeServer->send(200, "application/json", "{\"status\":\"ok\",\"enabled\":false}");
    }
  } else {
    activeServer->send(400, "application/json", "{status:error,message:Missing enable parameter}");
  }
}

void handleStatus() {
  if (!checkAuthentication()) return;
  ESP8266WebServer* activeServer = getCurrentServer();
  String json = "{";
  json += "\"wifi_mode\":\"" + String(isAPMode ? "AP" : "Station") + "\",";
  json += "\"ssid\":\"" + (isAPMode ? String(AP_SSID) : currentSSID) + "\",";
  json += "\"ip\":\"" + (isAPMode ? "192.168.4.1" : WiFi.localIP().toString()) + "\",";
  json += "\"connected\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false");
  json += "}";
  activeServer->send(200, "application/json", json);
}

void handleGetWiFi() {
  if (!checkAuthentication()) return;
  ESP8266WebServer* activeServer = getCurrentServer();
  String json = "{";
  json += "\"ssid\":\"" + currentSSID + "\",";
  json += "\"mode\":\"" + String(isAPMode ? "AP" : "Station") + "\"";
  json += "}";
  activeServer->send(200, "application/json", json);
}

void handleSetWiFi() {
  if (!checkAuthentication()) return;
  ESP8266WebServer* activeServer = getCurrentServer();
  if (activeServer->hasArg("ssid") && activeServer->hasArg("password")) {
    String ssid = activeServer->arg("ssid");
    String password = activeServer->arg("password");

    saveWiFiCredentials(ssid, password);
    displayAction("WiFi saved");

    String json = "{\"status\":\"ok\",\"message\":\"WiFi credentials saved. Restarting...\"}";
    activeServer->send(200, "application/json", json);

    delay(1000);
    ESP.restart();
  } else {
    activeServer->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing parameters\"}");
  }
}

void handleScan() {
  if (!checkAuthentication()) return;
  ESP8266WebServer* activeServer = getCurrentServer();
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
  activeServer->send(200, "application/json", json);
}

// Script API Handlers
void handleListScripts() {
  if (!checkAuthentication()) return;
  ESP8266WebServer* activeServer = getCurrentServer();
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
  activeServer->send(200, "application/json", json);
}

void handleSaveScript() {
  if (!checkAuthentication()) return;
  ESP8266WebServer* activeServer = getCurrentServer();
  if (activeServer->hasArg("name") && activeServer->hasArg("script")) {
    String name = activeServer->arg("name");
    String script = activeServer->arg("script");

    if (name.length() == 0) {
      activeServer->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Script name cannot be empty\"}");
      return;
    }

    if (name.length() > MAX_SCRIPT_NAME_LEN) {
      activeServer->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Script name too long\"}");
      return;
    }

    if (saveScriptToFile(name, script)) {
      displayAction("Saved: " + name);
      activeServer->send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Script saved\"}");
    } else {
      activeServer->send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save script\"}");
    }
  } else {
    activeServer->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing name or script parameter\"}");
  }
}

void handleLoadScript() {
  if (!checkAuthentication()) return;
  ESP8266WebServer* activeServer = getCurrentServer();
  if (activeServer->hasArg("name")) {
    String name = activeServer->arg("name");
    String script = loadScriptFromFile(name);

    if (script.length() == 0) {
      activeServer->send(404, "application/json", "{\"status\":\"error\",\"message\":\"Script not found\"}");
      return;
    }

    displayAction("Loaded: " + name);

    String json = "{";
    json += "\"status\":\"ok\",";
    json += "\"name\":\"" + escapeJson(name) + "\",";
    json += "\"script\":\"" + escapeJson(script) + "\"";
    json += "}";

    activeServer->send(200, "application/json", json);
  } else {
    activeServer->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing name parameter\"}");
  }
}

void handleDeleteScript() {
  if (!checkAuthentication()) return;
  ESP8266WebServer* activeServer = getCurrentServer();
  if (activeServer->hasArg("name")) {
    String name = activeServer->arg("name");

    if (deleteScriptFile(name)) {
      displayAction("Deleted: " + name);
      activeServer->send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Script deleted\"}");
    } else {
      activeServer->send(404, "application/json", "{\"status\":\"error\",\"message\":\"Script not found\"}");
    }
  } else {
    activeServer->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing name parameter\"}");
  }
}