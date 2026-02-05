#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <vector>

struct WiFiNetwork {
  String ssid;
  String password;
};

void loadWiFiNetworks();
bool addWiFiNetwork(String ssid, String password);
void deleteWiFiNetwork(int index);
bool connectToWiFi(String ssid, String password);
bool connectToAnyWiFi();
void startAPMode();

extern std::vector<WiFiNetwork> knownNetworks;
extern String currentSSID;
extern String currentPassword;
extern bool isAPMode;

#endif //WIFI_MANAGER_H
