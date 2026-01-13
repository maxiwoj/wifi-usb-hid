/*
 * WiFi USB HID Control - ESP32-C3 with Built-in OLED
 *
 * This sketch runs on ESP32-C3 with built-in 0.42" OLED display
 * Combines both WiFi web server and USB HID functionality in one board
 *
 * Features:
 * - USB Keyboard and Mouse emulation (native ESP32-C3 USB)
 * - WiFi web server with REST API
 * - Built-in OLED display support (0.42" 72x40)
 * - DuckyScript parser
 * - Mouse Jiggler
 * - Quick Actions and Scripts
 * - File Management
 */

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"

#include "config.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "display_manager.h"
#include "littlefs_manager.h"
#include "ducky_parser.h"
#include "quick_scripts.h"
#include "hid_handler.h"

extern WebServer server;
extern USBHIDKeyboard Keyboard;
extern USBHIDMouse Mouse;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(100);
  Serial.println("ESP32-C3 WiFi USB HID is booting up...");

  // Initialize USB HID
  setupHID();

  // Initialize preferences (replaces EEPROM on ESP32)
  setupPreferences();

  // Initialize display
  setupDisplay();

  // Initialize LittleFS
  setupLittleFS();

  // Load and connect to WiFi
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

  // Setup web server
  setupWebServer();

  if (isAPMode) {
    Serial.println("AP Mode - IP: 192.168.4.1");
  } else {
    Serial.println("Station Mode - IP: " + WiFi.localIP().toString());
  }

  updateDisplayStatus();

  Serial.println("ESP32-C3 WiFi USB HID ready!");
}

void loop() {
  // Handle web server requests
  handleWebClients();

  // Handle mouse jiggler
  handleJiggler();
}
