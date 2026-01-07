#ifndef CONFIG_H
#define CONFIG_H

// EEPROM addresses (for WiFi credentials only)
#define EEPROM_SIZE 512
#define SSID_ADDR 0
#define PASS_ADDR 100

// LittleFS settings for script storage
#define MAX_SCRIPT_NAME_LEN 32

// WiFi AP settings
#define AP_SSID "USB-HID-Setup"
#define AP_PASS "HID_M4ster"

// WiFi connection timeout
#define WIFI_TIMEOUT 10000

// OLED Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  // I2C address for 128x64 OLED

// Standard I2C pins for ESP8266 (recommended)
// Note: GPIO16 (D0) does NOT support I2C! Use D1/D2 instead
#define OLED_SDA D3
#define OLED_SCL D4

#endif //CONFIG_H
