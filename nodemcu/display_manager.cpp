#include "display_manager.h"
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

// OLED Display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool displayAvailable = false;
String lastAction = "";
unsigned long lastActionTime = 0;


extern bool isAPMode;
extern String currentSSID;

// Internal function to show startup logo
void showStartupLogo() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Large title - centered (8 chars * 12px = 96px, (128-96)/2 = 16)
  display.setTextSize(2);
  display.setCursor(16, 8);
  display.println("WiFi HID");

  // Decorative separator - centered (9 chars * 6px = 54px, (128-54)/2 = 37)
  display.setTextSize(1);
  display.setCursor(37, 28);
  display.println("=========");

  // Subtitle - centered (11 chars * 6px = 66px, (128-66)/2 = 31)
  display.setCursor(31, 42);
  display.println("USB Control");

  // Status - centered (11 chars * 6px = 66px, (128-66)/2 = 31)
  display.setCursor(31, 54);
  display.println("Starting...");

  display.display();
  delay(1500);
}

void setupDisplay() {
    // Initialize I2C for OLED display
  Serial.println("Initializing I2C for OLED display...");
  Wire.begin(OLED_SDA, OLED_SCL);
  delay(100);  // Give I2C time to stabilize

  // Scan I2C bus to find display
  Serial.println("Scanning I2C bus...");
  byte error, address;
  int nDevices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
  }

  if (nDevices == 0) {
    Serial.println("No I2C devices found - check wiring!");
  } else {
    Serial.print("Found ");
    Serial.print(nDevices);
    Serial.println(" I2C device(s)");
  }

  // Try to initialize OLED display at 0x3C
  Serial.print("Attempting to initialize OLED at 0x");
  Serial.println(SCREEN_ADDRESS, HEX);

  // Note: display.begin() sometimes returns true even when display isn't connected
  // We need to verify it's actually working
  if (nDevices == 0) {
    Serial.println("No I2C devices detected - display likely not connected");
    Serial.println("Skipping display initialization");
    displayAvailable = false;
  } else {
    // I2C device found, try to initialize
    if (display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println("Display library initialized - showing startup logo...");

      displayAvailable = true;
      Serial.println("OLED display is working!");

      // Show formatted startup logo
      showStartupLogo();
    } else {
      Serial.println("Display initialization failed at 0x3C");

      // Try alternative address 0x3D
      Serial.println("Trying alternative address 0x3D...");
      if (display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
        Serial.println("Display library initialized at 0x3D - showing startup logo...");

        displayAvailable = true;
        Serial.println("OLED display working at 0x3D!");

        // Show formatted startup logo
        showStartupLogo();
      } else {
        Serial.println("Display not found at 0x3C or 0x3D");
        displayAvailable = false;
      }
    }
  }

  if (!displayAvailable) {
    Serial.println("=== DISPLAY TROUBLESHOOTING ===");
    Serial.println("Display not working. Please verify:");
    Serial.println("1. Display VCC -> NodeMCU 3.3V (NOT Vin!)");
    Serial.println("2. Display GND -> NodeMCU GND");
    Serial.println("3. Display SDA -> NodeMCU D1 (GPIO5)");
    Serial.println("4. Display SCL -> NodeMCU D0 (GPIO16)");
    Serial.println("5. All connections are secure");
    Serial.println("6. Display is powered on (check for LED)");
    Serial.println("Continuing without display...");
    Serial.println("==============================");
  }
}

// OLED Display Functions
void updateDisplayStatus() {
  if (!displayAvailable) return;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Title
  display.println("WiFi HID Control");
  display.println("----------------");

  // Mode and IP on same line - saves space
  if (isAPMode) {
    display.println("AP  192.168.4.1");
    display.print("WiFi:");
    display.println(AP_SSID);
    display.print("Pass:");
    display.println(AP_PASS);
  } else {
    display.print("ST  ");
    display.println(WiFi.localIP().toString());
    display.print("Net:");
    display.println(currentSSID);
    // Don't show password in station mode to save space
  }

  // Web Authentication - no label, just credentials
  display.print(WEB_AUTH_USER);
  // Use inverse color for separator to distinguish username/password
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.print("/");
  display.setTextColor(SSD1306_WHITE);
  display.println(WEB_AUTH_PASS);

  // Blank line for separation
  display.println();

  // Last action (if any) - now has dedicated space
  if (lastAction.length() > 0) {
    unsigned long timeSinceAction = millis() - lastActionTime;
    if (timeSinceAction < 3000) { // Show for 3 seconds
      display.print(">");
      display.println(lastAction);
    }
  }

  display.display();
}

void displayAction(String action) {
  if (!displayAvailable) return;

  lastAction = action;
  lastActionTime = millis();
  updateDisplayStatus();
}
