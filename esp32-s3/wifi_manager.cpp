#include "wifi_manager.h"
#include <WiFi.h>
#include <Preferences.h>
#include "config.h"

Preferences preferences;
String currentSSID = "";
String currentPassword = "";
bool isAPMode = false;

void setupPreferences() {
  preferences.begin(PREFS_NAMESPACE, false);
}

void loadWiFiCredentials() {
  currentSSID = preferences.getString("ssid", "");
  currentPassword = preferences.getString("password", "");
}

void saveWiFiCredentials(String ssid, String password) {
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  currentSSID = ssid;
  currentPassword = password;
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
  if (WiFi.softAP(AP_SSID, AP_PASS)) {
    Serial.println("AP Mode started");
    Serial.println("SSID: " + String(AP_SSID));
    Serial.println("Password: " + String(AP_PASS));
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP());
    isAPMode = true;
  } else {
    Serial.println("Failed to start AP Mode!");
    isAPMode = false;
  }
}
