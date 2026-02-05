#include "wifi_manager.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "config.h"
#include "eeprom_helpers.h"
#include <vector>

std::vector<WiFiNetwork> knownNetworks;
String currentSSID = "";
String currentPassword = "";
bool isAPMode = false;

void loadWiFiNetworks() {
  knownNetworks.clear();
  EEPROM.begin(EEPROM_SIZE);
  
  uint8_t magic = EEPROM.read(WIFI_MAGIC_ADDR);
  
  if (magic == WIFI_MAGIC_VAL) {
    uint8_t count = EEPROM.read(WIFI_COUNT_ADDR);
    if (count > MAX_WIFI_NETWORKS) count = MAX_WIFI_NETWORKS;
    
    for (int i = 0; i < count; i++) {
      int addr = i * WIFI_NET_SIZE;
      WiFiNetwork net;
      net.ssid = readStringFromEEPROM(addr, 32);
      net.password = readStringFromEEPROM(addr + 32, 64);
      if (net.ssid.length() > 0) {
        knownNetworks.push_back(net);
      }
    }
  }
}

bool addWiFiNetwork(String ssid, String password) {
  // Check if it already exists
  for (int i = 0; i < knownNetworks.size(); i++) {
    if (knownNetworks[i].ssid == ssid) {
      knownNetworks[i].password = password;
      writeStringToEEPROM(i * WIFI_NET_SIZE + 32, password, 64);
      EEPROM.commit();
      return true;
    }
  }

  if (knownNetworks.size() < MAX_WIFI_NETWORKS) {
    WiFiNetwork net = {ssid, password};
    knownNetworks.push_back(net);
    
    int index = knownNetworks.size() - 1;
    int addr = index * WIFI_NET_SIZE;
    writeStringToEEPROM(addr, ssid, 32);
    writeStringToEEPROM(addr + 32, password, 64);
    EEPROM.write(WIFI_COUNT_ADDR, knownNetworks.size());
    EEPROM.write(WIFI_MAGIC_ADDR, WIFI_MAGIC_VAL);
    EEPROM.commit();
    return true;
  }
  return false;
}

void deleteWiFiNetwork(int index) {
  if (index >= 0 && index < knownNetworks.size()) {
    knownNetworks.erase(knownNetworks.begin() + index);
    
    // Rewrite all networks in EEPROM
    for (int i = 0; i < knownNetworks.size(); i++) {
      int addr = i * WIFI_NET_SIZE;
      writeStringToEEPROM(addr, knownNetworks[i].ssid, 32);
      writeStringToEEPROM(addr + 32, knownNetworks[i].password, 64);
    }
    EEPROM.write(WIFI_COUNT_ADDR, knownNetworks.size());
    EEPROM.commit();
  }
}

bool connectToWiFi(String ssid, String password) {
  Serial.println("\nConnecting to: " + ssid);
  
  // Clean up previous connection attempts
  WiFi.disconnect();
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
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.println("AP Mode started");
  Serial.println("SSID: " + String(AP_SSID));
  Serial.println("Password: " + String(AP_PASS));
  Serial.println("IP: 192.168.4.1");
  isAPMode = true;
}
