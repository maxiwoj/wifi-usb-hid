#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>

void updateDisplayStatus();
void displayAction(String action);
void setupDisplay();

extern bool displayAvailable;
extern String lastAction;
extern unsigned long lastActionTime;


#endif //DISPLAY_MANAGER_H
