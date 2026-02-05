/*
 * HID Handler for ESP32-S3
 * Handles USB HID keyboard and mouse operations using ESP32-S3's native USB
 *
 * IMPORTANT: In Arduino IDE, set:
 *   Tools > USB Mode > "USB-OTG (TinyUSB)"
 *   Tools > USB CDC On Boot > "Enabled" (for Serial + HID)
 */

#include "hid_handler.h"
#include <Arduino.h>

// ESP32-S3 has native USB HID support
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"

// Create HID devices
USBHIDKeyboard Keyboard;
USBHIDMouse Mouse;

#define USB_HID_AVAILABLE 1

// LED pin - T-Dongle-S3 doesn't have standard LED_BUILTIN, use onboard LED
#ifndef LED_BUILTIN
#define LED_BUILTIN 39  // T-Dongle-S3 onboard LED (active LOW)
#endif
const int LED_PIN = LED_BUILTIN;

// Mouse Jiggler state
static bool jigglerEnabled = false;
static unsigned long lastJiggleTime = 0;
static unsigned long jiggleInterval = 2000; // 2 seconds (configurable)
static int jiggleDirection = 1;
static int jiggleDiameter = 2; // configurable diameter
static String jiggleType = "simple"; // simple, circles, random

void setupHID() {
  Serial.println("Initializing USB HID...");

  // Initialize HID devices first
  Keyboard.begin();
  Mouse.begin();

  // Then start USB stack
  USB.begin();

  // Give USB time to enumerate
  delay(1000);

  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // T-Dongle-S3 LED is active LOW, HIGH = off

  // Startup indication
  blinkLED(3, 200);

  Serial.println("USB HID initialized successfully");
  Serial.println("If HID is not working, check Arduino IDE settings:");
  Serial.println("  Tools > USB Mode > USB-OTG (TinyUSB)");
}

void updateJiggler() {
  if (!jigglerEnabled) return;

  unsigned long currentTime = millis();
  if (currentTime - lastJiggleTime >= jiggleInterval) {
    // Move mouse based on jiggle type
    if (jiggleType == "simple") {
      // Simple left-right movement
      Mouse.move(jiggleDiameter * jiggleDirection, 0);
      jiggleDirection *= -1; // Alternate direction
    }
    else if (jiggleType == "circles") {
      // Draw a complete circle
      for (int angle = 0; angle < 360; angle += 10) {
        float radians = angle * 3.14159 / 180.0;
        int x = (int)(cos(radians) * jiggleDiameter);
        int y = (int)(sin(radians) * jiggleDiameter);
        Mouse.move(x, y);
        delay(5);
      }
    }
    else if (jiggleType == "random") {
      // Random movement
      int x = random(-jiggleDiameter, jiggleDiameter + 1);
      int y = random(-jiggleDiameter, jiggleDiameter + 1);
      Mouse.move(x, y);
    }

    lastJiggleTime = currentTime;

    // Blink LED
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
  }
}

void enableJiggler(String type, int diameter, unsigned long interval) {
  jiggleType = type;
  jiggleDiameter = diameter;
  jiggleInterval = interval;
  jigglerEnabled = true;
  lastJiggleTime = millis();
  Serial.println("Jiggler enabled (type=" + type + ", diameter=" + String(diameter) + ", delay=" + String(interval) + ")");
  blinkLED(2, 100);
}

void disableJiggler() {
  jigglerEnabled = false;
  Serial.println("Jiggler disabled");
  digitalWrite(LED_PIN, LOW);
}

bool isJigglerEnabled() {
  return jigglerEnabled;
}

// Helper to convert string key name to HID keycode
uint8_t getHIDKeyCode(String key) {
  if (key.length() == 1) return key[0];
  key.toUpperCase();
  
  if (key == "CTRL") return KEY_LEFT_CTRL;
  if (key == "SHIFT") return KEY_LEFT_SHIFT;
  if (key == "ALT") return KEY_LEFT_ALT;
  if (key == "GUI") return KEY_LEFT_GUI;
  if (key == "ENTER") return KEY_RETURN;
  if (key == "ESC") return KEY_ESC;
  if (key == "BACKSPACE") return KEY_BACKSPACE;
  if (key == "TAB") return KEY_TAB;
  if (key == "SPACE") return ' ';
  if (key == "DELETE") return KEY_DELETE;
  if (key == "INSERT") return KEY_INSERT;
  if (key == "HOME") return KEY_HOME;
  if (key == "END") return KEY_END;
  if (key == "PAGEUP") return KEY_PAGE_UP;
  if (key == "PAGEDOWN") return KEY_PAGE_DOWN;
  if (key == "UP") return KEY_UP_ARROW;
  if (key == "DOWN") return KEY_DOWN_ARROW;
  if (key == "LEFT") return KEY_LEFT_ARROW;
  if (key == "RIGHT") return KEY_RIGHT_ARROW;
  if (key == "CAPSLOCK") return KEY_CAPS_LOCK;
  
  if (key.startsWith("F")) {
    int f = key.substring(1).toInt();
    if (f >= 1 && f <= 12) return KEY_F1 + (f - 1);
  }
  
  return 0;
}

void processHIDCommand(String cmd) {
  cmd.trim();

  // Key Capture commands
  if (cmd.startsWith("KEY_PRESS:")) {
    String keyStr = cmd.substring(10);
    uint8_t key = getHIDKeyCode(keyStr);
    if (key) {
      Keyboard.press(key);
      Serial.println("Press: " + keyStr);
    }
  }
  else if (cmd.startsWith("KEY_RELEASE:")) {
    String keyStr = cmd.substring(12);
    uint8_t key = getHIDKeyCode(keyStr);
    if (key) {
      Keyboard.release(key);
      Serial.println("Release: " + keyStr);
    }
  }
  else if (cmd == "KEY_RELEASE_ALL") {
    Keyboard.releaseAll();
    Serial.println("Release All");
  }

  // Mouse Jiggler control
  else if (cmd.startsWith("JIGGLE_ON")) {
    // Parse parameters: JIGGLE_ON <type> <diameter> <delay>
    // Example: JIGGLE_ON circles 5 3000
    int firstSpace = cmd.indexOf(' ');
    if (firstSpace > 0) {
      int secondSpace = cmd.indexOf(' ', firstSpace + 1);
      int thirdSpace = cmd.indexOf(' ', secondSpace + 1);

      if (secondSpace > 0 && thirdSpace > 0) {
        String type = cmd.substring(firstSpace + 1, secondSpace);
        int diameter = cmd.substring(secondSpace + 1, thirdSpace).toInt();
        unsigned long delay = cmd.substring(thirdSpace + 1).toInt();

        // Validate and apply settings
        type.toLowerCase();
        if (type == "simple" || type == "circles" || type == "random") {
          jiggleType = type;
        }

        if (diameter > 0 && diameter <= 100) {
          jiggleDiameter = diameter;
        }

        if (delay >= 100 && delay <= 60000) {
          jiggleInterval = delay;
        }
      }
    }

    enableJiggler(jiggleType, jiggleDiameter, jiggleInterval);
  }
  else if (cmd == "JIGGLE_OFF") {
    disableJiggler();
  }

  // Type commands
  else if (cmd.startsWith("TYPE:")) {
    String text = cmd.substring(5);
    Keyboard.print(text);
    Serial.println("Typed text");
  }
  else if (cmd.startsWith("TYPELN:")) {
    String text = cmd.substring(7);
    Keyboard.println(text);
    Serial.println("Typed text with enter");
  }

  // Special keys
  else if (cmd == "ENTER") {
    Keyboard.write(KEY_RETURN);
    Serial.println("Enter");
  }
  else if (cmd == "ESC") {
    Keyboard.write(KEY_ESC);
    Serial.println("Escape");
  }
  else if (cmd == "TAB") {
    Keyboard.write(KEY_TAB);
    Serial.println("Tab");
  }
  else if (cmd == "BACKSPACE") {
    Keyboard.write(KEY_BACKSPACE);
    Serial.println("Backspace");
  }
  else if (cmd == "DELETE") {
    Keyboard.write(KEY_DELETE);
    Serial.println("Delete");
  }

  // GUI (Windows/Command) combinations
  else if (cmd == "GUI_R") {
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('r');
    delay(100);
    Keyboard.releaseAll();
    Serial.println("GUI+R");
  }
  else if (cmd == "GUI_D") {
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('d');
    delay(100);
    Keyboard.releaseAll();
    Serial.println("GUI+D");
  }
  else if (cmd == "GUI_SPACE") {
    Keyboard.press(KEY_LEFT_GUI);
    delay(50);
    Keyboard.press(' ');
    delay(50);
    Keyboard.release(' ');
    delay(50);
    Keyboard.release(KEY_LEFT_GUI);
    Serial.println("GUI+Space");
  }
  else if (cmd == "GUI") {
    Keyboard.press(KEY_LEFT_GUI);
    delay(100);
    Keyboard.release(KEY_LEFT_GUI);
    Serial.println("GUI");
  }
  else if (cmd == "GUI_ALT_SPACE") {
    // Alternative Spotlight shortcut (Command+Option+Space)
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press(KEY_LEFT_ALT);
    delay(50);
    Keyboard.press(' ');
    delay(50);
    Keyboard.release(' ');
    delay(50);
    Keyboard.release(KEY_LEFT_ALT);
    Keyboard.release(KEY_LEFT_GUI);
    Serial.println("GUI+Alt+Space");
  }
  else if (cmd == "GUI_TAB") {
    // Command+Tab (macOS app switcher) or Windows+Tab
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press(KEY_TAB);
    delay(100);
    Keyboard.releaseAll();
    Serial.println("GUI+Tab");
  }
  else if (cmd == "GUI_H") {
    // Command+H (Hide app on macOS)
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('h');
    delay(100);
    Keyboard.releaseAll();
    Serial.println("GUI+H");
  }
  else if (cmd == "GUI_W") {
    // Command+W (Close window on macOS) or Windows+W
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('w');
    delay(100);
    Keyboard.releaseAll();
    Serial.println("GUI+W");
  }

  // Keyboard shortcuts
  else if (cmd == "ALT_TAB") {
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_TAB);
    delay(100);
    Keyboard.releaseAll();
    Serial.println("Alt+Tab");
  }
  else if (cmd == "CTRL_ALT_DEL") {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_DELETE);
    delay(100);
    Keyboard.releaseAll();
    Serial.println("Ctrl+Alt+Del");
  }
  else if (cmd == "CTRL_ALT_T") {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press('t');
    delay(100);
    Keyboard.releaseAll();
    Serial.println("Ctrl+Alt+T");
  }

  // CTRL combinations
  else if (cmd.startsWith("CTRL_")) {
    char key = cmd.charAt(5);
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(key);
    delay(100);
    Keyboard.releaseAll();
    Serial.println("Ctrl+" + String(key));
  }

  // ALT combinations
  else if (cmd.startsWith("ALT_")) {
    if (cmd == "ALT_F4") {
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_F4);
      delay(100);
      Keyboard.releaseAll();
      Serial.println("Alt+F4");
    } else {
      char key = cmd.charAt(4);
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(key);
      delay(100);
      Keyboard.releaseAll();
      Serial.println("Alt+" + String(key));
    }
  }

  // Arrow keys
  else if (cmd == "UP") {
    Keyboard.write(KEY_UP_ARROW);
    Serial.println("Up");
  }
  else if (cmd == "DOWN") {
    Keyboard.write(KEY_DOWN_ARROW);
    Serial.println("Down");
  }
  else if (cmd == "LEFT") {
    Keyboard.write(KEY_LEFT_ARROW);
    Serial.println("Left");
  }
  else if (cmd == "RIGHT") {
    Keyboard.write(KEY_RIGHT_ARROW);
    Serial.println("Right");
  }

  // Function keys (F1-F12)
  else if (cmd == "F1") {
    Keyboard.write(KEY_F1);
    Serial.println("F1");
  }
  else if (cmd == "F2") {
    Keyboard.write(KEY_F2);
    Serial.println("F2");
  }
  else if (cmd == "F3") {
    Keyboard.write(KEY_F3);
    Serial.println("F3");
  }
  else if (cmd == "F4") {
    Keyboard.write(KEY_F4);
    Serial.println("F4");
  }
  else if (cmd == "F5") {
    Keyboard.write(KEY_F5);
    Serial.println("F5");
  }
  else if (cmd == "F6") {
    Keyboard.write(KEY_F6);
    Serial.println("F6");
  }
  else if (cmd == "F7") {
    Keyboard.write(KEY_F7);
    Serial.println("F7");
  }
  else if (cmd == "F8") {
    Keyboard.write(KEY_F8);
    Serial.println("F8");
  }
  else if (cmd == "F9") {
    Keyboard.write(KEY_F9);
    Serial.println("F9");
  }
  else if (cmd == "F10") {
    Keyboard.write(KEY_F10);
    Serial.println("F10");
  }
  else if (cmd == "F11") {
    Keyboard.write(KEY_F11);
    Serial.println("F11");
  }
  else if (cmd == "F12") {
    Keyboard.write(KEY_F12);
    Serial.println("F12");
  }

  // Mouse movement
  else if (cmd.startsWith("MOUSE_MOVE:")) {
    int commaIndex = cmd.indexOf(',');
    if (commaIndex > 0) {
      int x = cmd.substring(11, commaIndex).toInt();
      int y = cmd.substring(commaIndex + 1).toInt();
      Mouse.move(x, y);
      Serial.println("Mouse moved");
    }
  }

  // Mouse clicks
  else if (cmd == "MOUSE_LEFT") {
    Mouse.click(MOUSE_LEFT);
    Serial.println("Left click");
  }
  else if (cmd == "MOUSE_RIGHT") {
    Mouse.click(MOUSE_RIGHT);
    Serial.println("Right click");
  }
  else if (cmd == "MOUSE_MIDDLE") {
    Mouse.click(MOUSE_MIDDLE);
    Serial.println("Middle click");
  }
  else if (cmd == "MOUSE_DOUBLE") {
    Mouse.click(MOUSE_LEFT);
    delay(50);
    Mouse.click(MOUSE_LEFT);
    Serial.println("Double click");
  }
  else if (cmd == "MOUSE_PRESS") {
    Mouse.press(MOUSE_LEFT);
    Serial.println("Mouse pressed");
  }
  else if (cmd == "MOUSE_RELEASE") {
    Mouse.release(MOUSE_LEFT);
    Serial.println("Mouse released");
  }

  // Mouse scroll
  else if (cmd.startsWith("SCROLL:")) {
    int amount = cmd.substring(7).toInt();
    Mouse.move(0, 0, amount);
    Serial.println("Scrolled");
  }

  // Delay
  else if (cmd.startsWith("DELAY:")) {
    int ms = cmd.substring(6).toInt();
    if (ms > 0 && ms <= 10000) { // Max 10 seconds
      delay(ms);
      Serial.println("Delayed");
    }
  }

  // Utility commands
  else if (cmd == "PING") {
    Serial.println("PONG");
  }
  else if (cmd == "STATUS") {
    String status = "STATUS:";
    status += jigglerEnabled ? "Jiggler=ON" : "Jiggler=OFF";
    status += ",USB=ENABLED";
    Serial.println(status);
  }
  else if (cmd == "LED_ON") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED on");
  }
  else if (cmd == "LED_OFF") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED off");
  }
  else if (cmd == "RESTART") {
    Serial.println("Restarting ESP32...");
    delay(500);
    ESP.restart();
  }

  // Unknown command
  else {
    Serial.println("ERROR: Unknown command - " + cmd);
  }
}

void blinkLED(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(delayMs);
    digitalWrite(LED_PIN, LOW);
    delay(delayMs);
  }
}
