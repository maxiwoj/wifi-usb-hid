#ifndef HID_HANDLER_H
#define HID_HANDLER_H

#include <Arduino.h>

// HID setup and initialization
void setupHID();

// Command processor (similar to pro-micro's processCommand)
void processHIDCommand(String cmd);

// Mouse jiggler functions
void handleJiggler();
void enableJiggler(String type, int diameter, unsigned long interval);
void disableJiggler();
bool isJigglerEnabled();

// LED indicator
void blinkLED(int times, int delayMs);

#endif // HID_HANDLER_H
