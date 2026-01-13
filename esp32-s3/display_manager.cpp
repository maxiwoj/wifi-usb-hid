#include "display_manager.h"
#include <WiFi.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "config.h"

// ST7735 LCD Display (0.96" - 80x160 pixels)
Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

bool displayAvailable = false;
String lastAction = "";
unsigned long lastActionTime = 0;

extern bool isAPMode;
extern String currentSSID;

// Internal function to show startup logo
void showStartupLogo() {
  display.fillScreen(ST77XX_BLACK);
  display.setTextColor(ST77XX_WHITE);
  display.setTextWrap(false);

  // Title (large text)
  display.setTextSize(2);
  display.setCursor(5, 20);
  display.println("WiFi");
  display.setCursor(5, 40);
  display.println("HID");

  // Subtitle (small text)
  display.setTextSize(1);
  display.setCursor(10, 70);
  display.setTextColor(ST77XX_CYAN);
  display.println("Booting...");

  delay(1500);
}

void setupDisplay() {
  Serial.println("Initializing ST7735 LCD display...");

  // Initialize backlight pin if defined
  #if TFT_BL >= 0
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);  // Turn on backlight
    Serial.println("Backlight enabled");
  #endif

  // Initialize display
  // Use initR(INITR_MINI160x80) for 0.96" 160x80 displays
  // Or initR(INITR_BLACKTAB) for standard 128x160 displays
  display.initR(INITR_MINI160x80);  // Initialize for 0.96" 80x160 display

  // Set rotation (adjust based on your mounting)
  display.setRotation(DISPLAY_ROTATION);

  displayAvailable = true;
  Serial.println("ST7735 LCD display initialized successfully!");

  // Show formatted startup logo
  showStartupLogo();

  // Test pattern (optional - comment out after testing)
  /*
  display.fillScreen(ST77XX_BLACK);
  display.drawRect(0, 0, display.width(), display.height(), ST77XX_WHITE);
  display.setCursor(5, 5);
  display.setTextSize(1);
  display.setTextColor(ST77XX_GREEN);
  display.print("Size: ");
  display.print(display.width());
  display.print("x");
  display.println(display.height());
  delay(2000);
  */
}

// ST7735 Display Functions - Optimized for 80x160 (or 160x80 in landscape)
void updateDisplayStatus() {
  if (!displayAvailable) return;

  display.fillScreen(ST77XX_BLACK);
  display.setTextSize(1);
  display.setTextWrap(false);

  int y = 2;  // Starting Y position
  int lineHeight = 10;

  // Line 1: Title
  display.setCursor(2, y);
  display.setTextColor(ST77XX_YELLOW);
  display.println("WiFi USB HID");
  y += lineHeight + 2;

  // Separator line
  display.drawFastHLine(0, y, display.width(), ST77XX_BLUE);
  y += 4;

  // Line 2: Mode and IP
  display.setCursor(2, y);
  if (isAPMode) {
    display.setTextColor(ST77XX_GREEN);
    display.print("AP: ");
    display.setTextColor(ST77XX_WHITE);
    display.println("192.168.4.1");
  } else {
    display.setTextColor(ST77XX_GREEN);
    display.print("IP: ");
    display.setTextColor(ST77XX_WHITE);
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
  display.setTextColor(ST77XX_GREEN);
  display.print("Net: ");
  display.setTextColor(ST77XX_WHITE);
  String ssid = isAPMode ? String(AP_SSID) : currentSSID;
  if (ssid.length() > 10) {
    ssid = ssid.substring(0, 7) + "...";
  }
  display.println(ssid);
  y += lineHeight;

  // Line 4: Web Auth
  display.setCursor(2, y);
  display.setTextColor(ST77XX_CYAN);
  display.print("U:");
  display.setTextColor(ST77XX_WHITE);
  display.print(WEB_AUTH_USER);
  y += lineHeight;

  display.setCursor(2, y);
  display.setTextColor(ST77XX_CYAN);
  display.print("P:");
  display.setTextColor(ST77XX_WHITE);
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
      display.drawFastHLine(0, y, display.width(), ST77XX_ORANGE);
      y += 4;

      display.setCursor(2, y);
      display.setTextColor(ST77XX_ORANGE);
      display.print("> ");
      display.setTextColor(ST77XX_WHITE);
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
