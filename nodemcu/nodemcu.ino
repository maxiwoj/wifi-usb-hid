#include <ESP8266WebServer.h>
#include <EEPROM.h>


#include "config.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "display_manager.h"
#include "littlefs_manager.h"
#include "ducky_parser.h"
#include "pro_micro.h"
#include "quick_scripts.h"

extern ESP8266WebServer server;

void setup() {
  Serial.begin(74880);
  delay(100);
  Serial.println("Device is booting up...");

  EEPROM.begin(EEPROM_SIZE);

  setupDisplay();

  setupLittleFS();

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

  setupWebServer();

  server.begin();
  Serial.println("Web server started");

  if (isAPMode) {
    Serial.println("AP Mode - IP: 192.168.4.1");
  } else {
    Serial.println("Station Mode - IP: " + WiFi.localIP().toString());
  }

  updateDisplayStatus();
}

void loop() {
  server.handleClient();
}