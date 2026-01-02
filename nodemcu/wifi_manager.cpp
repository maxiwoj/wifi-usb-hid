#include "wifi_manager.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "config.h"
#include "eeprom_helpers.h"

String currentSSID = "";
String currentPassword = "";
bool isAPMode = false;

void loadWiFiCredentials() {
  currentSSID = readStringFromEEPROM(SSID_ADDR, 32);
  currentPassword = readStringFromEEPROM(PASS_ADDR, 64);
}

void saveWiFiCredentials(String ssid, String password) {
  writeStringToEEPROM(SSID_ADDR, ssid, 32);
  writeStringToEEPROM(PASS_ADDR, password, 64);
  EEPROM.commit();
}

bool connectToWiFi(String ssid, String password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startTime > WIFI_TIMEOUT) {
      return false;
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.println("IP: " + WiFi.localIP().toString());
  isAPMode = false;
  return true;
}

void startAPMode() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.println("AP Mode started");
  Serial.println("SSID: " + String(AP_SSID));
  Serial.println("Password: " + String(AP_PASS));
  Serial.println("IP: 192.168.4.1");
  isAPMode = true;
}
