#ifndef CONFIG_H
#define CONFIG_H

// Preferences namespace for WiFi credentials (ESP32 uses Preferences instead of EEPROM)
#define PREFS_NAMESPACE "wifi-hid"
#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64
#define MAX_WIFI_NETWORKS 5

// LittleFS settings for script storage
#define MAX_SCRIPT_NAME_LEN 32

// WiFi AP settings 
#define AP_SSID "USB-HID-Setup"
#define AP_PASS "HID_M4ster"

// Web Authentication settings
#define WEB_AUTH_USER "admin"
#define WEB_AUTH_PASS "WiFi_HID!826"

// HTTPS Settings
// WARNING: HTTPS uses significant memory
// Only enable if you have sufficient free memory
// Set to 1 to enable HTTPS on port 443, 0 to disable (HTTP only on port 80)
#define ENABLE_HTTPS 0

// WiFi connection timeout
#define WIFI_TIMEOUT 10000

// ST7735 LCD Display settings (0.96" IPS LCD - typically 80x160 pixels)
// This board uses ST7735 controller, NOT SSD1306 OLED
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 160
#define DISPLAY_ROTATION 1  // 0=0°, 1=90°, 2=180°, 3=270° (adjust for your orientation)

// ST7735 Color definitions (RGB565 format)
#define ST77XX_BLACK      0x0000
#define ST77XX_WHITE      0xFFFF
#define ST77XX_RED        0xF800
#define ST77XX_GREEN      0x07E0
#define ST77XX_BLUE       0x001F
#define ST77XX_CYAN       0x07FF
#define ST77XX_MAGENTA    0xF81F
#define ST77XX_YELLOW     0xFFE0
#define ST77XX_ORANGE     0xFD20

// SPI pins for ST7735 display
// NOTE: These may vary - check your board's documentation!
// IMPORTANT: These defines are for reference or non-TFT_eSPI libraries. 
// TFT_eSPI uses its own 'User_Setup.h' file in the library folder. 
// You MUST ensure the library's User_Setup.h matches these pins!

// ===== LILYGO T-Dongle-S3 pinout =====
 #define TFT_CS    4    // Chip select
 #define TFT_RST   1    // Reset pin
 #define TFT_DC    2    // Data/Command pin (also called RS)
 #define TFT_MOSI  3    // SPI MOSI
 #define TFT_SCLK  5    // SPI Clock
 #define TFT_BL    38   // Backlight pin

// ===== Alternative pinout (other ESP32-S3 dongles) =====
// Uncomment these and comment out the above if using a different board
// #define TFT_CS    10   // Chip select
// #define TFT_RST   14   // Reset pin
// #define TFT_DC    13   // Data/Command pin
// #define TFT_MOSI  11   // SPI MOSI
// #define TFT_SCLK  12   // SPI Clock
// #define TFT_BL    9    // Backlight pin

// ===== SD Card Pins (SD_MMC) =====
#define SD_MMC_D0   14
#define SD_MMC_D1   17
#define SD_MMC_D2   21
#define SD_MMC_D3   18
#define SD_MMC_CLK  12
#define SD_MMC_CMD  16

// USB HID settings for ESP32-S3
#define USB_HID_ENABLED 1  // ESP32-S3 has full USB HID support
#define SERIAL_BAUD 115200 // USB Serial baud rate

#endif //CONFIG_H
