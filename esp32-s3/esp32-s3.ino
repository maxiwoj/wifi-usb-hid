/*
 * WiFi USB HID Control - ESP32-S3 with Built-in LCD
 *
 * This sketch runs on ESP32-S3 with built-in 0.96" ST7735 IPS LCD display
 * Combines both WiFi web server and USB HID functionality in one board
 *
 * Hardware: ESP32-S3 Dongle with 0.96" LCD IPS ST7735 (80x160 pixels)
 * Chip: ESP32-S3R8 (8MB PSRAM)
 *
 * Features:
 * - USB Keyboard and Mouse emulation (native ESP32-S3 USB OTG)
 * - WiFi web server with REST API
 * - Built-in ST7735 LCD display support (0.96" IPS, 80x160)
 * - DuckyScript parser
 * - Mouse Jiggler
 * - Quick Actions and Scripts
 * - File Management
 */

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

#include "config.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "display_manager.h"
#include "littlefs_manager.h"
#include "ducky_parser.h"
#include "quick_scripts.h"
#include "hid_handler.h"

extern WebServer server;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(100);
  Serial.println("ESP32-S3 WiFi USB HID is booting up...");

  // Initialize preferences (replaces EEPROM on ESP32)
  setupPreferences();


  // Initialize Storage (SD Card or LittleFS)
  setupStorage();

  // Initialize ST7735 LCD display
  setupDisplay();

  // Load and connect to WiFi
  loadWiFiNetworks();

  if (knownNetworks.size() > 0) {
    Serial.println("Attempting to connect to saved networks...");
    if (!connectToAnyWiFi()) {
      Serial.println("Failed to connect to any network, starting AP mode");
      startAPMode();
    }
  } else {
    Serial.println("No saved credentials, starting AP mode");
    startAPMode();
  }

  // Setup web server
  setupWebServer();

  // Initialize USB HID
  setupHID();

  if (isAPMode) {
    Serial.println("AP Mode - IP: 192.168.4.1");
  } else {
    Serial.println("Station Mode - IP: " + WiFi.localIP().toString());
  }

  updateDisplayStatus();

  Serial.println("ESP32-S3 WiFi USB HID ready!");
}

void loop() {
  // Handle web server requests
  handleWebClients();

  // Handle mouse jiggler
  updateJiggler();
}
