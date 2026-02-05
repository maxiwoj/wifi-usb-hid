#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <vector>

struct WiFiNetwork {
  String ssid;
  String password;
};

void setupPreferences();
void loadWiFiNetworks();
void deleteWiFiNetwork(int index);
bool saveWiFiCredentials(String ssid, String password);
bool connectToWiFi(String ssid, String password);
bool connectToAnyWiFi();
void startAPMode();

extern std::vector<WiFiNetwork> knownNetworks;
extern String currentSSID;
extern String currentPassword;
extern bool isAPMode;

#endif //WIFI_MANAGER_H
