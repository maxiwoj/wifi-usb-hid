/*
 * HID Handler for ESP32-C3
 * Handles USB HID keyboard and mouse operations using ESP32-C3's native USB
 */

#include "hid_handler.h"
#include <Arduino.h>

// Try to include ESP32 USB HID libraries
// These are available when "USB CDC On Boot" is enabled in Arduino IDE
#if defined(ARDUINO_USB_MODE) && ARDUINO_USB_MODE == 1
  // USB is enabled - use real USB HID classes
  #include "USB.h"
  #include "USBHIDKeyboard.h"
  #include "USBHIDMouse.h"

  USBHIDKeyboard Keyboard;
  USBHIDMouse Mouse;

  #define USB_HID_AVAILABLE 1
#else
  // USB not enabled - provide stub implementations
  #warning "USB Mode not enabled! Set Tools > USB Mode to 'USB-OTG (TinyUSB)' and enable 'USB CDC On Boot'"

  class StubUSB {
  public:
    void begin() { Serial.println("WARNING: USB HID not available - USB mode not enabled"); }
  };

  class StubKeyboard {
  public:
    void begin() {}
    void end() {}
    void press(uint8_t) {}
    void release(uint8_t) {}
    void releaseAll() {}
    void write(uint8_t) {}
    size_t print(const String& s) { return s.length(); }
    size_t println(const String& s) { return s.length() + 1; }
  };

  class StubMouse {
  public:
    void begin() {}
    void end() {}
    void move(int8_t x, int8_t y, int8_t wheel = 0) {}
    void press(uint8_t b = 1) {}
    void release(uint8_t b = 1) {}
    void click(uint8_t b = 1) {}
  };

  StubUSB USB;
  StubKeyboard Keyboard;
  StubMouse Mouse;

  #define USB_HID_AVAILABLE 0

  // Define missing key constants for stub mode
  #define KEY_LEFT_CTRL   0x80
  #define KEY_LEFT_SHIFT  0x81
  #define KEY_LEFT_ALT    0x82
  #define KEY_LEFT_GUI    0x83
  #define KEY_RIGHT_CTRL  0x84
  #define KEY_RIGHT_SHIFT 0x85
  #define KEY_RIGHT_ALT   0x86
  #define KEY_RIGHT_GUI   0x87
  #define KEY_UP_ARROW    0xDA
  #define KEY_DOWN_ARROW  0xD9
  #define KEY_LEFT_ARROW  0xD8
  #define KEY_RIGHT_ARROW 0xD7
  #define KEY_BACKSPACE   0xB2
  #define KEY_TAB         0xB3
  #define KEY_RETURN      0xB0
  #define KEY_ESC         0xB1
  #define KEY_DELETE      0xD4
  #define KEY_F1          0xC2
  #define KEY_F2          0xC3
  #define KEY_F3          0xC4
  #define KEY_F4          0xC5
  #define KEY_F5          0xC6
  #define KEY_F6          0xC7
  #define KEY_F7          0xC8
  #define KEY_F8          0xC9
  #define KEY_F9          0xCA
  #define KEY_F10         0xCB
  #define KEY_F11         0xCC
  #define KEY_F12         0xCD
  #define MOUSE_LEFT      0x01
  #define MOUSE_RIGHT     0x02
  #define MOUSE_MIDDLE    0x04
#endif

// LED pin
const int LED_PIN = LED_BUILTIN;

// Mouse Jiggler state
static bool jigglerEnabled = false;
static unsigned long lastJiggleTime = 0;
static unsigned long jiggleInterval = 2000; // 2 seconds (configurable)
static int jiggleDirection = 1;
static int jiggleDiameter = 2; // configurable diameter
static String jiggleType = "simple"; // simple, circles, random

void setupHID() {
  // Initialize USB HID
  Keyboard.begin();
  USB.begin();
  Mouse.begin();

  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Startup indication
  blinkLED(3, 200);

  #if USB_HID_AVAILABLE
    Serial.println("USB HID initialized successfully");
  #else
    Serial.println("USB HID stub mode - HID functions will not work!");
    Serial.println("To enable: Tools > USB Mode > 'USB-OTG (TinyUSB)'");
    Serial.println("          Tools > USB CDC On Boot > 'Enabled'");
  #endif
}

void handleJiggler() {
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

void processHIDCommand(String cmd) {
  cmd.trim();

  // Mouse Jiggler control
  if (cmd.startsWith("JIGGLE_ON")) {
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
    #if USB_HID_AVAILABLE
      status += ",USB=ENABLED";
    #else
      status += ",USB=DISABLED";
    #endif
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
