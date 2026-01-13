#include "display_manager.h"
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

// OLED Display (0.42" - 72x40 pixels)
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

  // Title (centered for 72px width)
  display.setTextSize(1);
  display.setCursor(6, 10);
  display.println("WiFi HID");

  display.setCursor(6, 22);
  display.println("Booting..");

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
    Serial.println("1. Display VCC -> ESP32-C3 3.3V");
    Serial.println("2. Display GND -> ESP32-C3 GND");
    Serial.print("3. Display SDA -> GPIO");
    Serial.println(OLED_SDA);
    Serial.print("4. Display SCL -> GPIO");
    Serial.println(OLED_SCL);
    Serial.println("5. All connections are secure");
    Serial.println("Continuing without display...");
    Serial.println("==============================");
  }
}

// OLED Display Functions - Optimized for 72x40 display
void updateDisplayStatus() {
  if (!displayAvailable) return;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Line 1: Title - shortened to fit
  display.println("WiFi-HID");

  // Line 2: Mode and IP (abbreviated)
  if (isAPMode) {
    display.println("AP:.4.1");
  } else {
    // Show only last octet of IP for space
    String ip = WiFi.localIP().toString();
    int lastDot = ip.lastIndexOf('.');
    display.print("ST:");
    display.println(ip.substring(lastDot + 1));
  }

  // Line 3: SSID (truncated if too long)
  String ssid = isAPMode ? String(AP_SSID) : currentSSID;
  if (ssid.length() > 12) {
    ssid = ssid.substring(0, 9) + "...";
  }
  display.println(ssid);

  // Line 4: Last action (if any)
  if (lastAction.length() > 0) {
    unsigned long timeSinceAction = millis() - lastActionTime;
    if (timeSinceAction < 3000) { // Show for 3 seconds
      String action = lastAction;
      if (action.length() > 11) {
        action = action.substring(0, 8) + "...";
      }
      display.print(">");
      display.println(action);
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
