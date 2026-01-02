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

// EEPROM addresses
#define EEPROM_SIZE 512
#define SSID_ADDR 0
#define PASS_ADDR 100

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

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(100);

  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);

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

// Web Server Setup
void setupWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/setup", HTTP_GET, handleSetup);
  server.on("/api/command", HTTP_POST, handleCommand);
  server.on("/api/script", HTTP_POST, handleScript);
  server.on("/api/jiggler", HTTP_GET, handleJiggler);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/wifi", HTTP_GET, handleGetWiFi);
  server.on("/api/wifi", HTTP_POST, handleSetWiFi);
  server.on("/api/scan", HTTP_GET, handleScan);
}

// Web Server Handlers
void handleRoot() {
  String html = getMainHTML();
  server.send(200, "text/html", html);
}

void handleSetup() {
  String html = getSetupHTML();
  server.send(200, "text/html", html);
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

// Communication with Pro Micro
void sendCommandToProMicro(String cmd) {
  Serial.println(cmd);
  Serial.flush();
}

// DuckyScript Parser
void executeDuckyScript(String script) {
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

// HTML Pages
String getMainHTML() {
  return R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>WiFi USB HID Control</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }

    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      padding: 20px;
    }

    .container {
      max-width: 1200px;
      margin: 0 auto;
    }

    .header {
      background: white;
      border-radius: 15px;
      padding: 20px;
      margin-bottom: 20px;
      box-shadow: 0 10px 30px rgba(0,0,0,0.2);
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-wrap: wrap;
    }

    .header h1 {
      color: #667eea;
      font-size: 28px;
    }

    .status-badge {
      background: #10b981;
      color: white;
      padding: 8px 16px;
      border-radius: 20px;
      font-size: 14px;
    }

    .card {
      background: white;
      border-radius: 15px;
      padding: 20px;
      margin-bottom: 20px;
      box-shadow: 0 10px 30px rgba(0,0,0,0.2);
    }

    .card h2 {
      color: #333;
      margin-bottom: 15px;
      font-size: 20px;
    }

    .btn {
      padding: 10px 20px;
      border: none;
      border-radius: 8px;
      cursor: pointer;
      font-size: 14px;
      font-weight: 600;
      transition: all 0.3s;
      margin: 5px;
    }

    .btn:hover {
      transform: translateY(-2px);
      box-shadow: 0 5px 15px rgba(0,0,0,0.2);
    }

    .btn-primary { background: #667eea; color: white; }
    .btn-success { background: #10b981; color: white; }
    .btn-danger { background: #ef4444; color: white; }
    .btn-warning { background: #f59e0b; color: white; }
    .btn-info { background: #3b82f6; color: white; }

    .toggle-switch {
      position: relative;
      width: 60px;
      height: 30px;
      background: #ccc;
      border-radius: 30px;
      cursor: pointer;
      transition: 0.3s;
    }

    .toggle-switch.active {
      background: #10b981;
    }

    .toggle-switch::after {
      content: '';
      position: absolute;
      width: 26px;
      height: 26px;
      background: white;
      border-radius: 50%;
      top: 2px;
      left: 2px;
      transition: 0.3s;
    }

    .toggle-switch.active::after {
      left: 32px;
    }

    .jiggler-control {
      display: flex;
      align-items: center;
      gap: 15px;
    }

    .button-grid {
      display: grid;
      grid-template-columns: repeat(auto-fill, minmax(120px, 1fr));
      gap: 10px;
    }

    .mouse-grid {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 10px;
      max-width: 300px;
    }

    .mouse-grid .btn:nth-child(2),
    .mouse-grid .btn:nth-child(5),
    .mouse-grid .btn:nth-child(8) {
      grid-column: 2;
    }

    input[type="text"],
    textarea,
    select {
      width: 100%;
      padding: 10px;
      border: 2px solid #e5e7eb;
      border-radius: 8px;
      font-size: 14px;
      margin-bottom: 10px;
    }

    textarea {
      min-height: 150px;
      font-family: monospace;
      resize: vertical;
    }

    .activity-log {
      max-height: 200px;
      overflow-y: auto;
      background: #f9fafb;
      padding: 10px;
      border-radius: 8px;
      font-family: monospace;
      font-size: 12px;
    }

    .log-entry {
      padding: 5px 0;
      border-bottom: 1px solid #e5e7eb;
    }

    .log-time {
      color: #6b7280;
      margin-right: 10px;
    }

    .trackpad {
      width: 100%;
      max-width: 400px;
      height: 300px;
      background: linear-gradient(135deg, #f3f4f6 0%, #e5e7eb 100%);
      border: 3px solid #667eea;
      border-radius: 15px;
      margin: 15px auto;
      cursor: crosshair;
      position: relative;
      touch-action: none;
      user-select: none;
    }

    .trackpad-info {
      position: absolute;
      top: 10px;
      left: 50%;
      transform: translateX(-50%);
      background: rgba(102, 126, 234, 0.9);
      color: white;
      padding: 5px 15px;
      border-radius: 20px;
      font-size: 12px;
      pointer-events: none;
    }

    .trackpad-cursor {
      position: absolute;
      width: 20px;
      height: 20px;
      background: #667eea;
      border: 2px solid white;
      border-radius: 50%;
      pointer-events: none;
      transform: translate(-50%, -50%);
      opacity: 0;
      transition: opacity 0.2s;
    }

    .trackpad.active .trackpad-cursor {
      opacity: 1;
    }

    .sensitivity-control {
      display: flex;
      align-items: center;
      gap: 10px;
      margin: 10px 0;
    }

    .sensitivity-control input[type="range"] {
      flex: 1;
      margin: 0;
    }

    @media (max-width: 768px) {
      .button-grid {
        grid-template-columns: repeat(2, 1fr);
      }

      .trackpad {
        height: 250px;
      }
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <h1>WiFi USB HID Control</h1>
      <div style="display: flex; gap: 10px; align-items: center;">
        <span class="status-badge" id="statusBadge">Connected</span>
        <button class="btn btn-info" onclick="location.href='/setup'">WiFi Settings</button>
      </div>
    </div>

    <div class="card">
      <h2>Mouse Jiggler</h2>
      <div class="jiggler-control">
        <div class="toggle-switch" id="jigglerToggle" onclick="toggleJiggler()"></div>
        <span id="jigglerStatus">Disabled</span>
      </div>
    </div>

    <div class="card">
      <h2>Quick Actions</h2>
      <div class="button-grid">
        <button class="btn btn-primary" onclick="sendCommand('GUI_R')">GUI+R</button>
        <button class="btn btn-primary" onclick="sendCommand('GUI SPACE')">GUI+Space</button>
        <button class="btn btn-primary" onclick="sendCommand('GUI_D')">GUI+D</button>
        <button class="btn btn-primary" onclick="sendCommand('ALT_TAB')">Alt+Tab</button>
        <button class="btn btn-success" onclick="sendCommand('ENTER')">Enter</button>
        <button class="btn btn-warning" onclick="sendCommand('ESC')">Escape</button>
        <button class="btn btn-info" onclick="sendCommand('TAB')">Tab</button>
      </div>
    </div>

    <div class="card">
      <h2>Type Text</h2>
      <input type="text" id="typeText" placeholder="Enter text to type...">
      <button class="btn btn-primary" onclick="typeText()">Type</button>
      <button class="btn btn-success" onclick="typeTextEnter()">Type + Enter</button>
    </div>

    <div class="card">
      <h2>Mouse Control</h2>

      <h3 style="margin-bottom: 10px; font-size: 16px; color: #667eea;">Live Trackpad</h3>
      <div class="trackpad" id="trackpad">
        <div class="trackpad-info">Drag to move mouse • Click to click • Double-click to double-click</div>
        <div class="trackpad-cursor" id="trackpadCursor"></div>
      </div>

      <div class="sensitivity-control">
        <label style="min-width: 80px;">Sensitivity:</label>
        <input type="range" id="sensitivity" min="0.5" max="3" step="0.1" value="1.5">
        <span id="sensitivityValue">1.5x</span>
      </div>

      <h3 style="margin: 20px 0 10px 0; font-size: 16px; color: #667eea;">Directional Control</h3>
      <div class="mouse-grid">
        <button class="btn btn-info" onclick="sendCommand('MOUSE_MOVE:-10,-10')">↖</button>
        <button class="btn btn-info" onclick="sendCommand('MOUSE_MOVE:0,-10')">↑</button>
        <button class="btn btn-info" onclick="sendCommand('MOUSE_MOVE:10,-10')">↗</button>
        <button class="btn btn-info" onclick="sendCommand('MOUSE_MOVE:-10,0')">←</button>
        <button class="btn btn-success" onclick="sendCommand('MOUSE_LEFT')">Click</button>
        <button class="btn btn-info" onclick="sendCommand('MOUSE_MOVE:10,0')">→</button>
        <button class="btn btn-info" onclick="sendCommand('MOUSE_MOVE:-10,10')">↙</button>
        <button class="btn btn-info" onclick="sendCommand('MOUSE_MOVE:0,10')">↓</button>
        <button class="btn btn-info" onclick="sendCommand('MOUSE_MOVE:10,10')">↘</button>
      </div>
      <div style="margin-top: 10px;">
        <button class="btn btn-success" onclick="sendCommand('MOUSE_LEFT')">Left Click</button>
        <button class="btn btn-warning" onclick="sendCommand('MOUSE_RIGHT')">Right Click</button>
        <button class="btn btn-primary" onclick="sendCommand('MOUSE_DOUBLE')">Double Click</button>
      </div>
    </div>

    <div class="card">
      <h2>OS Selection</h2>
      <select id="osSelect">
        <option value="Windows">Windows</option>
        <option value="MacOS">MacOS</option>
        <option value="Linux">Linux</option>
      </select>
    </div>

    <div class="card">
      <h2>Quick Scripts</h2>
      <div class="button-grid">
        <button class="btn btn-primary" onclick="runQuickScript('editor')">Open Editor</button>
        <button class="btn btn-success" onclick="runQuickScript('terminal')">Terminal</button>
        <button class="btn btn-warning" onclick="runQuickScript('calculator')">Calculator</button>
        <button class="btn btn-info" onclick="runQuickScript('browser')">Browser</button>
      </div>
    </div>

    <div class="card">
      <h2>DuckyScript Executor</h2>
      <textarea id="duckyScript" placeholder="Enter DuckyScript here...
Example:
GUI r
DELAY 500
STRING notepad
ENTER
DELAY 1000
STRING Hello World!"></textarea>
      <button class="btn btn-danger" onclick="executeScript()">Execute Script</button>
    </div>

    <div class="card">
      <h2>Activity Log</h2>
      <div class="activity-log" id="activityLog"></div>
    </div>
  </div>

  <script>
    let jigglerEnabled = false;

    function log(message) {
      const logDiv = document.getElementById('activityLog');
      const time = new Date().toLocaleTimeString();
      const entry = document.createElement('div');
      entry.className = 'log-entry';
      entry.innerHTML = '<span class="log-time">' + time + '</span>' + message;
      logDiv.insertBefore(entry, logDiv.firstChild);

      // Keep only last 50 entries
      while (logDiv.children.length > 50) {
        logDiv.removeChild(logDiv.lastChild);
      }
    }

    function sendCommand(cmd) {
      fetch('/api/command', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'cmd=' + encodeURIComponent(cmd)
      })
      .then(response => response.json())
      .then(data => {
        log('Command sent: ' + cmd);
      })
      .catch(error => {
        log('Error: ' + error);
      });
    }

    function toggleJiggler() {
      jigglerEnabled = !jigglerEnabled;
      const toggle = document.getElementById('jigglerToggle');
      const status = document.getElementById('jigglerStatus');

      if (jigglerEnabled) {
        toggle.classList.add('active');
        status.textContent = 'Enabled';
      } else {
        toggle.classList.remove('active');
        status.textContent = 'Disabled';
      }

      fetch('/api/jiggler?enable=' + (jigglerEnabled ? '1' : '0'))
        .then(response => response.json())
        .then(data => {
          log('Mouse jiggler ' + (jigglerEnabled ? 'enabled' : 'disabled'));
        });
    }

    function typeText() {
      const text = document.getElementById('typeText').value;
      if (text) {
        sendCommand('TYPE:' + text);
        document.getElementById('typeText').value = '';
      }
    }

    function typeTextEnter() {
      const text = document.getElementById('typeText').value;
      if (text) {
        sendCommand('TYPELN:' + text);
        document.getElementById('typeText').value = '';
      }
    }

    function runQuickScript(scriptName) {
      const os = document.getElementById('osSelect').value;
      const scripts = {
        'Windows': {
          'editor': 'GUI r\nDELAY 500\nSTRING notepad\nENTER',
          'terminal': 'GUI r\nDELAY 500\nSTRING cmd\nENTER',
          'calculator': 'GUI r\nDELAY 500\nSTRING calc\nENTER',
          'browser': 'GUI r\nDELAY 500\nSTRING chrome\nENTER'
        },
        'MacOS': {
          'editor': 'GUI SPACE\nDELAY 500\nSTRING textedit\nENTER',
          'terminal': 'GUI SPACE\nDELAY 500\nSTRING terminal\nENTER',
          'calculator': 'GUI SPACE\nDELAY 500\nSTRING calculator\nENTER',
          'browser': 'GUI SPACE\nDELAY 500\nSTRING safari\nENTER'
        },
        'Linux': {
          'editor': 'CTRL ALT T\nDELAY 1000\nSTRING gedit\nENTER',
          'terminal': 'CTRL ALT T',
          'calculator': 'GUI\nDELAY 500\nSTRING calc\nENTER',
          'browser': 'GUI\nDELAY 500\nSTRING firefox\nENTER'
        }
      };

      const script = scripts[os][scriptName];
      if (script) {
        executeScriptText(script);
      }
    }

    function executeScript() {
      const script = document.getElementById('duckyScript').value;
      executeScriptText(script);
    }

    function executeScriptText(script) {
      fetch('/api/script', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'script=' + encodeURIComponent(script)
      })
      .then(response => response.json())
      .then(data => {
        log('Script executed');
      })
      .catch(error => {
        log('Error: ' + error);
      });
    }

    // Initial log
    log('Interface loaded - Ready to use');
  </script>
</body>
</html>
)=====";
}

String getSetupHTML() {
  return R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>WiFi Setup - USB HID Control</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }

    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      padding: 20px;
    }

    .container {
      max-width: 600px;
      margin: 0 auto;
    }

    .card {
      background: white;
      border-radius: 15px;
      padding: 30px;
      margin-bottom: 20px;
      box-shadow: 0 10px 30px rgba(0,0,0,0.2);
    }

    h1 {
      color: #667eea;
      margin-bottom: 20px;
    }

    h2 {
      color: #333;
      margin-bottom: 15px;
      font-size: 18px;
    }

    .btn {
      padding: 12px 24px;
      border: none;
      border-radius: 8px;
      cursor: pointer;
      font-size: 14px;
      font-weight: 600;
      transition: all 0.3s;
      margin: 5px 0;
      width: 100%;
    }

    .btn:hover {
      transform: translateY(-2px);
      box-shadow: 0 5px 15px rgba(0,0,0,0.2);
    }

    .btn-primary { background: #667eea; color: white; }
    .btn-success { background: #10b981; color: white; }
    .btn-info { background: #3b82f6; color: white; }

    input {
      width: 100%;
      padding: 12px;
      border: 2px solid #e5e7eb;
      border-radius: 8px;
      font-size: 14px;
      margin-bottom: 15px;
    }

    .network-list {
      max-height: 300px;
      overflow-y: auto;
      margin: 15px 0;
    }

    .network-item {
      padding: 12px;
      border: 2px solid #e5e7eb;
      border-radius: 8px;
      margin-bottom: 10px;
      cursor: pointer;
      transition: all 0.3s;
    }

    .network-item:hover {
      background: #f3f4f6;
      border-color: #667eea;
    }

    .network-item.selected {
      background: #ede9fe;
      border-color: #667eea;
    }

    .current-network {
      background: #d1fae5;
      padding: 15px;
      border-radius: 8px;
      margin-bottom: 15px;
    }

    .status-message {
      padding: 15px;
      border-radius: 8px;
      margin: 15px 0;
      display: none;
    }

    .status-success {
      background: #d1fae5;
      color: #065f46;
    }

    .status-error {
      background: #fee2e2;
      color: #991b1b;
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="card">
      <h1>WiFi Configuration</h1>

      <div id="currentNetwork" class="current-network">
        <strong>Current Network:</strong> <span id="currentSSID">Loading...</span>
      </div>

      <button class="btn btn-info" onclick="scanNetworks()">Scan Networks</button>

      <div id="networkList" class="network-list"></div>

      <h2>WiFi Credentials</h2>
      <input type="text" id="ssid" placeholder="Network SSID">
      <input type="password" id="password" placeholder="Password">

      <button class="btn btn-success" onclick="saveWiFi()">Save & Connect</button>
      <button class="btn btn-primary" onclick="location.href='/'">Back to Control</button>

      <div id="statusMessage" class="status-message"></div>
    </div>
  </div>

  <script>
    let selectedSSID = '';

    function loadCurrentNetwork() {
      fetch('/api/wifi')
        .then(response => response.json())
        .then(data => {
          document.getElementById('currentSSID').textContent = data.ssid || 'None';
        });
    }

    function scanNetworks() {
      const networkList = document.getElementById('networkList');
      networkList.innerHTML = '<p>Scanning...</p>';

      fetch('/api/scan')
        .then(response => response.json())
        .then(networks => {
          if (networks.length === 0) {
            networkList.innerHTML = '<p>No networks found</p>';
            return;
          }

          networkList.innerHTML = '';
          networks.forEach(network => {
            const item = document.createElement('div');
            item.className = 'network-item';
            item.innerHTML = '<strong>' + network.ssid + '</strong><br>Signal: ' + network.rssi + ' dBm';
            item.onclick = function() {
              selectNetwork(network.ssid, item);
            };
            networkList.appendChild(item);
          });
        })
        .catch(error => {
          networkList.innerHTML = '<p>Error scanning networks</p>';
        });
    }

    function selectNetwork(ssid, element) {
      selectedSSID = ssid;
      document.getElementById('ssid').value = ssid;

      // Remove previous selection
      const items = document.querySelectorAll('.network-item');
      items.forEach(item => item.classList.remove('selected'));

      // Add selection to clicked item
      element.classList.add('selected');
    }

    function saveWiFi() {
      const ssid = document.getElementById('ssid').value;
      const password = document.getElementById('password').value;

      if (!ssid) {
        showStatus('Please enter a network SSID', 'error');
        return;
      }

      fetch('/api/wifi', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'ssid=' + encodeURIComponent(ssid) + '&password=' + encodeURIComponent(password)
      })
      .then(response => response.json())
      .then(data => {
        showStatus('WiFi credentials saved! Device will restart...', 'success');
        setTimeout(() => {
          location.href = '/';
        }, 3000);
      })
      .catch(error => {
        showStatus('Error saving credentials', 'error');
      });
    }

    function showStatus(message, type) {
      const statusDiv = document.getElementById('statusMessage');
      statusDiv.textContent = message;
      statusDiv.className = 'status-message status-' + type;
      statusDiv.style.display = 'block';

      setTimeout(() => {
        statusDiv.style.display = 'none';
      }, 5000);
    }

    // Load current network on page load
    loadCurrentNetwork();
  </script>
</body>
</html>
)=====";
}
