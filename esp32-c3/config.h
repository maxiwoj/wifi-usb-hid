#ifndef CONFIG_H
#define CONFIG_H

// Preferences namespace for WiFi credentials (ESP32 uses Preferences instead of EEPROM)
#define PREFS_NAMESPACE "wifi-hid"
#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64

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

// OLED Display settings (0.42" OLED - typically 72x40 pixels)
// Note: Check your specific board's OLED specs, some 0.42" displays are 72x40
#define SCREEN_WIDTH 72
#define SCREEN_HEIGHT 40
#define OLED_RESET -1  // Reset pin (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  // I2C address for OLED (try 0x3D if 0x3C doesn't work)

// I2C pins for ESP32-C3
// NOTE: These pins may vary depending on your specific board!
// Common configurations:
// - Some boards: SDA=GPIO8, SCL=GPIO9
// - Other boards: SDA=GPIO5, SCL=GPIO6
// Check your board's pinout documentation
#define OLED_SDA 8
#define OLED_SCL 9

// USB HID settings for ESP32-C3
#define USB_HID_ENABLED 1  // ESP32-C3 has native USB support
#define SERIAL_BAUD 115200 // USB Serial baud rate

#endif //CONFIG_H
