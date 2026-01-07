#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>

void setupWebServer();
void handleWebClients();
bool checkAuthentication();
void handleRoot();
void handleSetup();
void handleCSS();
void handleJS();
void handleCommand();
void handleScript();
void handleJiggler();
void handleStatus();
void handleGetWiFi();
void handleSetWiFi();
void handleScan();
void handleListScripts();
void handleSaveScript();
void handleLoadScript();
void handleDeleteScript();

#endif //WEB_SERVER_H
