#include "wifi_manager.h"
#include <WiFi.h>
#include <Preferences.h>
#include "config.h"
#include <vector>

Preferences preferences;
std::vector<WiFiNetwork> knownNetworks;
String currentSSID = "";
String currentPassword = "";
bool isAPMode = false;

void setupPreferences() {
  preferences.begin(PREFS_NAMESPACE, false);
}

void loadWiFiNetworks() {
  knownNetworks.clear();
  int count = preferences.getInt("wifi_count", 0);
  
  // Backward compatibility: check if "ssid" exists from old version
  String oldSSID = preferences.getString("ssid", "");
  String oldPass = preferences.getString("password", "");
  
  if (count == 0 && oldSSID.length() > 0) {
    WiFiNetwork net = {oldSSID, oldPass};
    knownNetworks.push_back(net);
    // Migrate to new format
    preferences.putInt("wifi_count", 1);
    preferences.putString("ssid0", oldSSID);
    preferences.putString("pass0", oldPass);
    // Remove old keys to complete migration
    preferences.remove("ssid");
    preferences.remove("password");
  } else {
    for (int i = 0; i < count; i++) {
      String ssidKey = "ssid" + String(i);
      String passKey = "pass" + String(i);
      WiFiNetwork net;
      net.ssid = preferences.getString(ssidKey.c_str(), "");
      net.password = preferences.getString(passKey.c_str(), "");
      if (net.ssid.length() > 0) {
        knownNetworks.push_back(net);
      }
    }
  }
}

void addWiFiNetwork(String ssid, String password) {
  // Check if it already exists
  for (const auto& net : knownNetworks) {
    if (net.ssid == ssid) {
      // Update password if SSID exists
      saveWiFiCredentials(ssid, password);
      return;
    }
  }

  if (knownNetworks.size() < MAX_WIFI_NETWORKS) {
    WiFiNetwork net = {ssid, password};
    knownNetworks.push_back(net);
    
    int index = knownNetworks.size() - 1;
    preferences.putString(("ssid" + String(index)).c_str(), ssid);
    preferences.putString(("pass" + String(index)).c_str(), password);
    preferences.putInt("wifi_count", knownNetworks.size());
  } else {
    // If full, replace the last one or just don't add? 
    // Let's replace the last one for now or just return.
    // Usually it's better to tell the user it's full.
    Serial.println("Max WiFi networks reached");
  }
}

void deleteWiFiNetwork(int index) {
  if (index >= 0 && index < knownNetworks.size()) {
    knownNetworks.erase(knownNetworks.begin() + index);
    
    // Rewrite all networks in preferences
    preferences.putInt("wifi_count", knownNetworks.size());
    for (int i = 0; i < knownNetworks.size(); i++) {
      preferences.putString(("ssid" + String(i)).c_str(), knownNetworks[i].ssid);
      preferences.putString(("pass" + String(i)).c_str(), knownNetworks[i].password);
    }
    // Remove the last key that's no longer used
    preferences.remove(("ssid" + String(knownNetworks.size())).c_str());
    preferences.remove(("pass" + String(knownNetworks.size())).c_str());
  }
}

void saveWiFiCredentials(String ssid, String password) {
  // Check if it exists and update, otherwise add
  bool found = false;
  for (int i = 0; i < knownNetworks.size(); i++) {
    if (knownNetworks[i].ssid == ssid) {
      knownNetworks[i].password = password;
      preferences.putString(("pass" + String(i)).c_str(), password);
      found = true;
      break;
    }
  }
  
  if (!found) {
    addWiFiNetwork(ssid, password);
  }
}

bool connectToWiFi(String ssid, String password) {
  Serial.println("\nConnecting to: " + ssid);
  
  // Clean up previous connection attempts
  WiFi.disconnect(true);
  delay(100);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startTime > WIFI_TIMEOUT) {
      Serial.println("\nConnection timed out");
      return false;
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.println("IP: " + WiFi.localIP().toString());
  isAPMode = false;
  currentSSID = ssid;
  currentPassword = password;
  return true;
}

bool connectToAnyWiFi() {
  if (knownNetworks.empty()) {
    return false;
  }

  Serial.println("Scanning for available networks...");
  int n = WiFi.scanNetworks();
  Serial.println("Scan done, found " + String(n) + " networks");

  if (n <= 0) {
    Serial.println("No networks found in scan, but trying known networks anyway...");
    for (const auto& net : knownNetworks) {
      if (connectToWiFi(net.ssid, net.password)) {
        return true;
      }
    }
    return false;
  }

  // Iterate through available networks and check if they are in our known list
  for (int i = 0; i < n; i++) {
    String foundSSID = WiFi.SSID(i);
    for (const auto& net : knownNetworks) {
      if (net.ssid == foundSSID) {
        Serial.println("Found known network: " + foundSSID);
        if (connectToWiFi(net.ssid, net.password)) {
          return true;
        }
      }
    }
  }

  Serial.println("None of the known networks were available or could be connected to.");
  return false;
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
