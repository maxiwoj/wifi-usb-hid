#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>

void setupWebServer();
void handleWebClients();
bool checkAuthentication();

// API Handlers
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
void handleListQuickActions();
void handleSaveQuickAction();
void handleDeleteQuickAction();
void handleReorderQuickActions();
void handleListCustomOS();
void handleSaveCustomOS();
void handleDeleteCustomOS();
void handleListQuickScripts();
void handleSaveQuickScript();
void handleDeleteQuickScript();
void handleListFiles();
void handleFileUpload();
void handleFileUploadDone();
void handleFileDelete();
void handleFileDownload();
void handleCreateDir();

#endif //WEB_SERVER_H