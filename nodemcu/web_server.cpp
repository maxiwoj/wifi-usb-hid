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

ESP8266WebServer server(80);

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
    server.send(400, "application/json", "{status:error,message:Missing cmd parameter}");
  }
}

void handleScript() {
  if (server.hasArg("script")) {
    String script = server.arg("script");
    executeDuckyScript(script);
    displayAction("Script executed");
    server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Script executed\"}");
  } else {
    server.send(400, "application/json", "{status:error,message:Missing script parameter}");
  }
}

void handleJiggler() {
  if (server.hasArg("enable")) {
    String enable = server.arg("enable");
    if (enable == "1") {
      sendCommandToProMicro("JIGGLE_ON");
      displayAction("Jiggler ON");
      server.send(200, "application/json", "{\"status\":\"ok\",\"enabled\":true}");
    } else {
      sendCommandToProMicro("JIGGLE_OFF");
      displayAction("Jiggler OFF");
      server.send(200, "application/json", "{\"status\":\"ok\",\"enabled\":false}");
    }
  } else {
    server.send(400, "application/json", "{status:error,message:Missing enable parameter}");
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
    displayAction("WiFi saved");

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
      displayAction("Saved: " + name);
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

    displayAction("Loaded: " + name);

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
      displayAction("Deleted: " + name);
      server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Script deleted\"}");
    } else {
      server.send(404, "application/json", "{\"status\":\"error\",\"message\":\"Script not found\"}");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing name parameter\"}");
  }
}