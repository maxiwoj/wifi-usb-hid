#include "display_manager.h"
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "littlefs_manager.h"

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

// Helper to read 2 or 4 byte values from file (Little Endian)
uint16_t read16(fs::File &f) {
  uint16_t result;
  f.read((uint8_t *)&result, sizeof(result));
  return result;
}

uint32_t read32(fs::File &f) {
  uint32_t result;
  f.read((uint8_t *)&result, sizeof(result));
  return result;
}

bool drawBmp(const char *filename, int16_t x, int16_t y) {
  if (!storageAvailable || !storageFS) return false;

  fs::File bmpFile = storageFS->open(filename, "r");
  if (!bmpFile) {
    Serial.println("BMP file not found: " + String(filename));
    return false;
  }

  // Check BMP signature
  if (read16(bmpFile) != 0x4D42) {
    Serial.println("Not a valid BMP file");
    bmpFile.close();
    return false;
  }

  read32(bmpFile); // File size
  read32(bmpFile); // Reserved
  uint32_t offset = read32(bmpFile); // Start of image data
  uint32_t headerSize = read32(bmpFile); // Header size
  int32_t width = read32(bmpFile);
  int32_t height = read32(bmpFile);
  uint16_t planes = read16(bmpFile);
  uint16_t depth = read16(bmpFile);
  uint32_t compression = read32(bmpFile);

  // Support BI_RGB (0) and BI_BITFIELDS (3)
  if (planes != 1 || (depth != 24 && depth != 32) || (compression != 0 && compression != 3)) {
    Serial.print("Unsupported BMP: Depth="); Serial.print(depth);
    Serial.print(", Comp="); Serial.println(compression);
    bmpFile.close();
    return false;
  }

  bool topDown = (height < 0);
  if (topDown) height = -height;

  bmpFile.seek(offset);

  int bytesPerPixel = depth / 8;
  int rowPadding = (4 - (width * bytesPerPixel) % 4) % 4;
  uint16_t lineBuffer[width];

  display.startWrite();
  display.setSwapBytes(true); // Ensure 16-bit colors are swapped for ST7735
  
  for (int row = 0; row < height; row++) {
    // Determine target Y based on orientation
    int32_t targetY = topDown ? (y + row) : (y + height - 1 - row);
    
    // Skip if outside screen bounds
    if (targetY < 0 || targetY >= display.height()) {
       bmpFile.seek(bmpFile.position() + (width * bytesPerPixel) + rowPadding);
       continue;
    }

    for (int col = 0; col < width; col++) {
      uint8_t b = bmpFile.read();
      uint8_t g = bmpFile.read();
      uint8_t r = bmpFile.read();
      if (depth == 32) bmpFile.read(); // Skip alpha
      
      // BMP is usually BGR. color565 expects R, G, B
      lineBuffer[col] = display.color565(r, g, b);
    }
    bmpFile.seek(bmpFile.position() + rowPadding);
    display.pushImage(x, targetY, width, 1, lineBuffer);
  }
  display.setSwapBytes(false); // Reset to default
  display.endWrite();

  bmpFile.close();
  return true;
}

// Internal function to show startup logo
void showStartupLogo() {
  display.fillScreen(COLOR_BLACK);
  
  const char* logoPath = "/logo.bmp";
  if (!storageFS->exists(logoPath)) {
    logoPath = "/www/logo.bmp";
  }

  bool logoDrawn = false;
  if (storageAvailable && storageFS && storageFS->exists(logoPath)) {
    // Read dimensions for centering
    fs::File file = storageFS->open(logoPath, "r");
    if (file) {
      file.seek(18);
      int32_t w = read32(file);
      int32_t h = read32(file);
      if (h < 0) h = -h;
      file.close();
      
      int16_t x = (display.width() - w) / 2;
      int16_t y = (display.height() - h) / 2;
      logoDrawn = drawBmp(logoPath, x, y);
    }
  }

  if (!logoDrawn) {
    // Fallback if no BMP found
    display.setTextColor(COLOR_WHITE);
    display.setTextSize(2);
    display.setCursor(10, 20);
    display.println("WiFi HID");
    display.setTextSize(1);
    display.setCursor(40, 50);
    display.setTextColor(COLOR_CYAN);
    display.println("Booting...");
  }

  delay(2000);
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
  int maxChars = 25; // Approx chars for 160px width with 6px font

  // Line 1: Title
  display.setCursor(2, y);
  display.setTextColor(COLOR_YELLOW);
  display.println("WiFi USB HID Control");
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
    // IP usually fits in 160px
    display.println(ip);
  }
  y += lineHeight;

  // Line 3: SSID
  display.setCursor(2, y);
  display.setTextColor(COLOR_GREEN);
  display.print("Net: ");
  display.setTextColor(COLOR_WHITE);
  String ssid = isAPMode ? String(AP_SSID) : currentSSID;
  if (ssid.length() > maxChars - 5) {
    ssid = ssid.substring(0, maxChars - 8) + "...";
  }
  display.println(ssid);
  y += lineHeight;

  // Line 4: Web Auth
  display.setCursor(2, y);
  display.setTextColor(COLOR_CYAN);
  display.print("U:");
  display.setTextColor(COLOR_WHITE);
  display.print(WEB_AUTH_USER);
  
  // Display Pass on same line if short, or next line?
  // Let's keep separate lines but use full width
  y += lineHeight;
  
  display.setCursor(2, y);
  display.setTextColor(COLOR_CYAN);
  display.print("P:");
  display.setTextColor(COLOR_WHITE);
  String pass = String(WEB_AUTH_PASS);
  if (pass.length() > maxChars - 3) {
    pass = pass.substring(0, maxChars - 6) + "...";
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
      if (action.length() > maxChars - 2) {
        action = action.substring(0, maxChars - 5) + "...";
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
