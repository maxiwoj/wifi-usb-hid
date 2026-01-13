#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

void setupPreferences();
void loadWiFiCredentials();
void saveWiFiCredentials(String ssid, String password);
bool connectToWiFi(String ssid, String password);
void startAPMode();

extern String currentSSID;
extern String currentPassword;
extern bool isAPMode;

#endif //WIFI_MANAGER_H
