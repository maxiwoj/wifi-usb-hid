#include "display_manager.h"
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "config.h"

// TFT_eSPI display instance
TFT_eSPI display = TFT_eSPI();

bool displayAvailable = false;
String lastAction = "";
unsigned long lastActionTime = 0;

extern bool isAPMode;
extern String currentSSID;

// Color definitions for TFT_eSPI
#define COLOR_BLACK   TFT_BLACK
#define COLOR_WHITE   TFT_WHITE
#define COLOR_RED     TFT_RED
#define COLOR_GREEN   TFT_GREEN
#define COLOR_BLUE    TFT_BLUE
#define COLOR_CYAN    TFT_CYAN
#define COLOR_YELLOW  TFT_YELLOW
#define COLOR_ORANGE  TFT_ORANGE

// Internal function to show startup logo
void showStartupLogo() {
  display.fillScreen(COLOR_BLACK);
  display.setTextColor(COLOR_WHITE);

  // Title (large text)
  display.setTextSize(2);
  display.setCursor(5, 20);
  display.println("WiFi");
  display.setCursor(5, 40);
  display.println("HID");

  // Subtitle (small text)
  display.setTextSize(1);
  display.setCursor(10, 70);
  display.setTextColor(COLOR_CYAN);
  display.println("Booting...");

  delay(1500);
}

void setupDisplay() {
  Serial.println("Initializing TFT display with TFT_eSPI...");

  // Initialize display
  display.init();

  // Set rotation for T-Dongle-S3 (typically 1 or 3 for landscape)
  display.setRotation(DISPLAY_ROTATION);

  displayAvailable = true;
  Serial.println("TFT display initialized!");
  Serial.print("Display size: ");
  Serial.print(display.width());
  Serial.print("x");
  Serial.println(display.height());

  // Test pattern - shows colored rectangles to verify display is working
  Serial.println("Showing test pattern...");
  display.fillScreen(COLOR_RED);
  delay(500);
  display.fillScreen(COLOR_GREEN);
  delay(500);
  display.fillScreen(COLOR_BLUE);
  delay(500);
  display.fillScreen(COLOR_WHITE);
  delay(500);
  Serial.println("Test pattern complete");

  // Show formatted startup logo
  showStartupLogo();
}

// Display Functions - Optimized for 80x160 (or 160x80 in landscape)
void updateDisplayStatus() {
  if (!displayAvailable) return;

  display.fillScreen(COLOR_BLACK);
  display.setTextSize(1);

  int y = 2;  // Starting Y position
  int lineHeight = 10;

  // Line 1: Title
  display.setCursor(2, y);
  display.setTextColor(COLOR_YELLOW);
  display.println("WiFi USB HID");
  y += lineHeight + 2;

  // Separator line
  display.drawFastHLine(0, y, display.width(), COLOR_BLUE);
  y += 4;

  // Line 2: Mode and IP
  display.setCursor(2, y);
  if (isAPMode) {
    display.setTextColor(COLOR_GREEN);
    display.print("AP: ");
    display.setTextColor(COLOR_WHITE);
    display.println("192.168.4.1");
  } else {
    display.setTextColor(COLOR_GREEN);
    display.print("IP: ");
    display.setTextColor(COLOR_WHITE);
    String ip = WiFi.localIP().toString();
    // Truncate IP if too long
    if (ip.length() > 12) {
      int lastDot = ip.lastIndexOf('.');
      ip = "..." + ip.substring(lastDot);
    }
    display.println(ip);
  }
  y += lineHeight;

  // Line 3: SSID
  display.setCursor(2, y);
  display.setTextColor(COLOR_GREEN);
  display.print("Net: ");
  display.setTextColor(COLOR_WHITE);
  String ssid = isAPMode ? String(AP_SSID) : currentSSID;
  if (ssid.length() > 10) {
    ssid = ssid.substring(0, 7) + "...";
  }
  display.println(ssid);
  y += lineHeight;

  // Line 4: Web Auth
  display.setCursor(2, y);
  display.setTextColor(COLOR_CYAN);
  display.print("U:");
  display.setTextColor(COLOR_WHITE);
  display.print(WEB_AUTH_USER);
  y += lineHeight;

  display.setCursor(2, y);
  display.setTextColor(COLOR_CYAN);
  display.print("P:");
  display.setTextColor(COLOR_WHITE);
  String pass = String(WEB_AUTH_PASS);
  if (pass.length() > 11) {
    pass = pass.substring(0, 8) + "...";
  }
  display.println(pass);
  y += lineHeight + 4;

  // Last action (if any)
  if (lastAction.length() > 0) {
    unsigned long timeSinceAction = millis() - lastActionTime;
    if (timeSinceAction < 3000) { // Show for 3 seconds
      display.drawFastHLine(0, y, display.width(), COLOR_ORANGE);
      y += 4;

      display.setCursor(2, y);
      display.setTextColor(COLOR_ORANGE);
      display.print("> ");
      display.setTextColor(COLOR_WHITE);
      String action = lastAction;
      if (action.length() > 11) {
        action = action.substring(0, 8) + "...";
      }
      display.println(action);
    }
  }
}

void displayAction(String action) {
  if (!displayAvailable) return;

  lastAction = action;
  lastActionTime = millis();
  updateDisplayStatus();
}
