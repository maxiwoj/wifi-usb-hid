/*
 * WiFi USB HID Control - Pro Micro
 *
 * This sketch runs on Arduino Pro Micro (ATmega32U4)
 * Receives commands via Serial1 from NodeMCU and executes them as USB HID actions
 *
 * Features:
 * - USB Keyboard and Mouse emulation
 * - Mouse Jiggler (auto mouse movement)
 * - Command parser for various HID actions
 * - LED status indicator
 */

#include <Keyboard.h>
#include <Mouse.h>

// Pin definitions
const int LED_PIN = LED_BUILTIN;

// Mouse Jiggler state
bool jigglerEnabled = false;
unsigned long lastJiggleTime = 0;
unsigned long jiggleInterval = 2000; // 2 seconds (configurable)
int jiggleDirection = 1;
int jiggleDiameter = 2; // configurable diameter
String jiggleType = "simple"; // simple, circles, random

// Command buffer
String commandBuffer = "";

void setup() {
  // Initialize Serial1 for communication with NodeMCU (TX=1, RX=0)
  Serial1.begin(74880);

  // Initialize USB HID
  Keyboard.begin();
  Mouse.begin();

  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Startup indication
  blinkLED(3, 200);

  Serial1.println("READY");
}

void loop() {
  // Read commands from NodeMCU
  while (Serial1.available()) {
    char c = Serial1.read();

    if (c == '\n') {
      // Process complete command
      commandBuffer.trim();
      if (commandBuffer.length() > 0) {
        processCommand(commandBuffer);
      }
      commandBuffer = "";
    } else {
      commandBuffer += c;
    }
  }

  // Handle mouse jiggler
  if (jigglerEnabled) {
    unsigned long currentTime = millis();
    if (currentTime - lastJiggleTime >= jiggleInterval) {
      // Move mouse based on jiggle type
      if (jiggleType == "simple") {
        // Simple left-right movement
        Mouse.move(jiggleDiameter * jiggleDirection, 0, 0);
        jiggleDirection *= -1; // Alternate direction
      }
      else if (jiggleType == "circles") {
        // Draw a complete circle
        for (int angle = 0; angle < 360; angle += 10) {
          float radians = angle * 3.14159 / 180.0;
          int x = (int)(cos(radians) * jiggleDiameter);
          int y = (int)(sin(radians) * jiggleDiameter);
          Mouse.move(x, y, 0);
          delay(5);
        }
      }
      else if (jiggleType == "random") {
        // Random movement
        int x = random(-jiggleDiameter, jiggleDiameter + 1);
        int y = random(-jiggleDiameter, jiggleDiameter + 1);
        Mouse.move(x, y, 0);
      }

      lastJiggleTime = currentTime;

      // Blink LED
      digitalWrite(LED_PIN, HIGH);
      delay(50);
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void processCommand(String cmd) {
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

    jigglerEnabled = true;
    lastJiggleTime = millis();
    Serial1.println("OK:Jiggler enabled (type=" + jiggleType + ", diameter=" + String(jiggleDiameter) + ", delay=" + String(jiggleInterval) + ")");
    blinkLED(2, 100);
  }
  else if (cmd == "JIGGLE_OFF") {
    jigglerEnabled = false;
    Serial1.println("OK:Jiggler disabled");
    digitalWrite(LED_PIN, LOW);
  }

  // Type commands
  else if (cmd.startsWith("TYPE:")) {
    String text = cmd.substring(5);
    Keyboard.print(text);
    Serial1.println("OK:Typed text");
  }
  else if (cmd.startsWith("TYPELN:")) {
    String text = cmd.substring(7);
    Keyboard.println(text);
    Serial1.println("OK:Typed text with enter");
  }

  // Special keys
  else if (cmd == "ENTER") {
    Keyboard.press(KEY_RETURN);
    Keyboard.release(KEY_RETURN);
    Serial1.println("OK:Enter");
  }
  else if (cmd == "ESC") {
    Keyboard.press(KEY_ESC);
    Keyboard.release(KEY_ESC);
    Serial1.println("OK:Escape");
  }
  else if (cmd == "TAB") {
    Keyboard.press(KEY_TAB);
    Keyboard.release(KEY_TAB);
    Serial1.println("OK:Tab");
  }
  else if (cmd == "BACKSPACE") {
    Keyboard.press(KEY_BACKSPACE);
    Keyboard.release(KEY_BACKSPACE);
    Serial1.println("OK:Backspace");
  }
  else if (cmd == "DELETE") {
    Keyboard.press(KEY_DELETE);
    Keyboard.release(KEY_DELETE);
    Serial1.println("OK:Delete");
  }

  // GUI (Windows/Command) combinations
  else if (cmd == "GUI_R") {
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('r');
    delay(100);
    Keyboard.releaseAll();
    Serial1.println("OK:GUI+R");
  }
  else if (cmd == "GUI_D") {
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('d');
    delay(100);
    Keyboard.releaseAll();
    Serial1.println("OK:GUI+D");
  }
  else if (cmd == "GUI_SPACE") {
    Keyboard.press(KEY_LEFT_GUI);
    delay(50);
    Keyboard.press(' ');
    delay(50);
    Keyboard.release(' ');
    delay(50);
    Keyboard.release(KEY_LEFT_GUI);
    Serial1.println("OK:GUI+Space");
  }
  else if (cmd == "GUI") {
    Keyboard.press(KEY_LEFT_GUI);
    delay(100);
    Keyboard.release(KEY_LEFT_GUI);
    Serial1.println("OK:GUI");
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
    Serial1.println("OK:GUI+Alt+Space");
  }
  else if (cmd == "GUI_TAB") {
    // Command+Tab (macOS app switcher) or Windows+Tab
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press(KEY_TAB);
    delay(100);
    Keyboard.releaseAll();
    Serial1.println("OK:GUI+Tab");
  }
  else if (cmd == "GUI_H") {
    // Command+H (Hide app on macOS)
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('h');
    delay(100);
    Keyboard.releaseAll();
    Serial1.println("OK:GUI+H");
  }
  else if (cmd == "GUI_W") {
    // Command+W (Close window on macOS) or Windows+W
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('w');
    delay(100);
    Keyboard.releaseAll();
    Serial1.println("OK:GUI+W");
  }

  // Keyboard shortcuts
  else if (cmd == "ALT_TAB") {
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_TAB);
    delay(100);
    Keyboard.releaseAll();
    Serial1.println("OK:Alt+Tab");
  }
  else if (cmd == "CTRL_ALT_DEL") {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_DELETE);
    delay(100);
    Keyboard.releaseAll();
    Serial1.println("OK:Ctrl+Alt+Del");
  }
  else if (cmd == "CTRL_ALT_T") {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press('t');
    delay(100);
    Keyboard.releaseAll();
    Serial1.println("OK:Ctrl+Alt+T");
  }

  // CTRL combinations
  else if (cmd.startsWith("CTRL_")) {
    char key = cmd.charAt(5);
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(key);
    delay(100);
    Keyboard.releaseAll();
    Serial1.println("OK:Ctrl+" + String(key));
  }

  // ALT combinations
  else if (cmd.startsWith("ALT_")) {
    char key = cmd.charAt(4);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(key);
    delay(100);
    Keyboard.releaseAll();
    Serial1.println("OK:Alt+" + String(key));
  }

  // Arrow keys
  else if (cmd == "UP") {
    Keyboard.press(KEY_UP_ARROW);
    Keyboard.release(KEY_UP_ARROW);
    Serial1.println("OK:Up");
  }
  else if (cmd == "DOWN") {
    Keyboard.press(KEY_DOWN_ARROW);
    Keyboard.release(KEY_DOWN_ARROW);
    Serial1.println("OK:Down");
  }
  else if (cmd == "LEFT") {
    Keyboard.press(KEY_LEFT_ARROW);
    Keyboard.release(KEY_LEFT_ARROW);
    Serial1.println("OK:Left");
  }
  else if (cmd == "RIGHT") {
    Keyboard.press(KEY_RIGHT_ARROW);
    Keyboard.release(KEY_RIGHT_ARROW);
    Serial1.println("OK:Right");
  }

  // Function keys (F1-F12)
  else if (cmd == "F1") {
    Keyboard.press(KEY_F1);
    Keyboard.release(KEY_F1);
    Serial1.println("OK:F1");
  }
  else if (cmd == "F2") {
    Keyboard.press(KEY_F2);
    Keyboard.release(KEY_F2);
    Serial1.println("OK:F2");
  }
  else if (cmd == "F3") {
    Keyboard.press(KEY_F3);
    Keyboard.release(KEY_F3);
    Serial1.println("OK:F3");
  }
  else if (cmd == "F4") {
    Keyboard.press(KEY_F4);
    Keyboard.release(KEY_F4);
    Serial1.println("OK:F4");
  }
  else if (cmd == "F5") {
    Keyboard.press(KEY_F5);
    Keyboard.release(KEY_F5);
    Serial1.println("OK:F5");
  }
  else if (cmd == "F6") {
    Keyboard.press(KEY_F6);
    Keyboard.release(KEY_F6);
    Serial1.println("OK:F6");
  }
  else if (cmd == "F7") {
    Keyboard.press(KEY_F7);
    Keyboard.release(KEY_F7);
    Serial1.println("OK:F7");
  }
  else if (cmd == "F8") {
    Keyboard.press(KEY_F8);
    Keyboard.release(KEY_F8);
    Serial1.println("OK:F8");
  }
  else if (cmd == "F9") {
    Keyboard.press(KEY_F9);
    Keyboard.release(KEY_F9);
    Serial1.println("OK:F9");
  }
  else if (cmd == "F10") {
    Keyboard.press(KEY_F10);
    Keyboard.release(KEY_F10);
    Serial1.println("OK:F10");
  }
  else if (cmd == "F11") {
    Keyboard.press(KEY_F11);
    Keyboard.release(KEY_F11);
    Serial1.println("OK:F11");
  }
  else if (cmd == "F12") {
    Keyboard.press(KEY_F12);
    Keyboard.release(KEY_F12);
    Serial1.println("OK:F12");
  }

  // Mouse movement
  else if (cmd.startsWith("MOUSE_MOVE:")) {
    int commaIndex = cmd.indexOf(',');
    if (commaIndex > 0) {
      int x = cmd.substring(11, commaIndex).toInt();
      int y = cmd.substring(commaIndex + 1).toInt();
      Mouse.move(x, y, 0);
      Serial1.println("OK:Mouse moved");
    }
  }

  // Mouse clicks
  else if (cmd == "MOUSE_LEFT") {
    Mouse.click(MOUSE_LEFT);
    Serial1.println("OK:Left click");
  }
  else if (cmd == "MOUSE_RIGHT") {
    Mouse.click(MOUSE_RIGHT);
    Serial1.println("OK:Right click");
  }
  else if (cmd == "MOUSE_MIDDLE") {
    Mouse.click(MOUSE_MIDDLE);
    Serial1.println("OK:Middle click");
  }
  else if (cmd == "MOUSE_DOUBLE") {
    Mouse.click(MOUSE_LEFT);
    delay(50);
    Mouse.click(MOUSE_LEFT);
    Serial1.println("OK:Double click");
  }
  else if (cmd == "MOUSE_PRESS") {
    Mouse.press(MOUSE_LEFT);
    Serial1.println("OK:Mouse pressed");
  }
  else if (cmd == "MOUSE_RELEASE") {
    Mouse.release(MOUSE_LEFT);
    Serial1.println("OK:Mouse released");
  }

  // Mouse scroll
  else if (cmd.startsWith("SCROLL:")) {
    int amount = cmd.substring(7).toInt();
    Mouse.move(0, 0, amount);
    Serial1.println("OK:Scrolled");
  }

  // Key press/release for multi-key combinations
  else if (cmd.startsWith("KEY_PRESS:")) {
    String key = cmd.substring(10);
    key.trim();

    // Handle special keys
    if (key == "CTRL") {
      Keyboard.press(KEY_LEFT_CTRL);
    }
    else if (key == "SHIFT") {
      Keyboard.press(KEY_LEFT_SHIFT);
    }
    else if (key == "ALT") {
      Keyboard.press(KEY_LEFT_ALT);
    }
    else if (key == "GUI" || key == "META" || key == "WIN" || key == "CMD") {
      Keyboard.press(KEY_LEFT_GUI);
    }
    else if (key == "ENTER") {
      Keyboard.press(KEY_RETURN);
    }
    else if (key == "ESC") {
      Keyboard.press(KEY_ESC);
    }
    else if (key == "TAB") {
      Keyboard.press(KEY_TAB);
    }
    else if (key == "BACKSPACE") {
      Keyboard.press(KEY_BACKSPACE);
    }
    else if (key == "DELETE") {
      Keyboard.press(KEY_DELETE);
    }
    else if (key == "SPACE") {
      Keyboard.press(' ');
    }
    else if (key == "UP") {
      Keyboard.press(KEY_UP_ARROW);
    }
    else if (key == "DOWN") {
      Keyboard.press(KEY_DOWN_ARROW);
    }
    else if (key == "LEFT") {
      Keyboard.press(KEY_LEFT_ARROW);
    }
    else if (key == "RIGHT") {
      Keyboard.press(KEY_RIGHT_ARROW);
    }
    else if (key == "HOME") {
      Keyboard.press(KEY_HOME);
    }
    else if (key == "END") {
      Keyboard.press(KEY_END);
    }
    else if (key == "PAGEUP") {
      Keyboard.press(KEY_PAGE_UP);
    }
    else if (key == "PAGEDOWN") {
      Keyboard.press(KEY_PAGE_DOWN);
    }
    else if (key == "INSERT") {
      Keyboard.press(KEY_INSERT);
    }
    else if (key == "CAPSLOCK") {
      Keyboard.press(KEY_CAPS_LOCK);
    }
    // Function keys
    else if (key == "F1") {
      Keyboard.press(KEY_F1);
    }
    else if (key == "F2") {
      Keyboard.press(KEY_F2);
    }
    else if (key == "F3") {
      Keyboard.press(KEY_F3);
    }
    else if (key == "F4") {
      Keyboard.press(KEY_F4);
    }
    else if (key == "F5") {
      Keyboard.press(KEY_F5);
    }
    else if (key == "F6") {
      Keyboard.press(KEY_F6);
    }
    else if (key == "F7") {
      Keyboard.press(KEY_F7);
    }
    else if (key == "F8") {
      Keyboard.press(KEY_F8);
    }
    else if (key == "F9") {
      Keyboard.press(KEY_F9);
    }
    else if (key == "F10") {
      Keyboard.press(KEY_F10);
    }
    else if (key == "F11") {
      Keyboard.press(KEY_F11);
    }
    else if (key == "F12") {
      Keyboard.press(KEY_F12);
    }
    // Regular character (single char)
    else if (key.length() == 1) {
      Keyboard.press(key.charAt(0));
    }

    Serial1.println("OK:Key pressed: " + key);
  }
  else if (cmd.startsWith("KEY_RELEASE:")) {
    String key = cmd.substring(12);
    key.trim();

    // Handle special keys
    if (key == "CTRL") {
      Keyboard.release(KEY_LEFT_CTRL);
    }
    else if (key == "SHIFT") {
      Keyboard.release(KEY_LEFT_SHIFT);
    }
    else if (key == "ALT") {
      Keyboard.release(KEY_LEFT_ALT);
    }
    else if (key == "GUI" || key == "META" || key == "WIN" || key == "CMD") {
      Keyboard.release(KEY_LEFT_GUI);
    }
    else if (key == "ENTER") {
      Keyboard.release(KEY_RETURN);
    }
    else if (key == "ESC") {
      Keyboard.release(KEY_ESC);
    }
    else if (key == "TAB") {
      Keyboard.release(KEY_TAB);
    }
    else if (key == "BACKSPACE") {
      Keyboard.release(KEY_BACKSPACE);
    }
    else if (key == "DELETE") {
      Keyboard.release(KEY_DELETE);
    }
    else if (key == "SPACE") {
      Keyboard.release(' ');
    }
    else if (key == "UP") {
      Keyboard.release(KEY_UP_ARROW);
    }
    else if (key == "DOWN") {
      Keyboard.release(KEY_DOWN_ARROW);
    }
    else if (key == "LEFT") {
      Keyboard.release(KEY_LEFT_ARROW);
    }
    else if (key == "RIGHT") {
      Keyboard.release(KEY_RIGHT_ARROW);
    }
    else if (key == "HOME") {
      Keyboard.release(KEY_HOME);
    }
    else if (key == "END") {
      Keyboard.release(KEY_END);
    }
    else if (key == "PAGEUP") {
      Keyboard.release(KEY_PAGE_UP);
    }
    else if (key == "PAGEDOWN") {
      Keyboard.release(KEY_PAGE_DOWN);
    }
    else if (key == "INSERT") {
      Keyboard.release(KEY_INSERT);
    }
    else if (key == "CAPSLOCK") {
      Keyboard.release(KEY_CAPS_LOCK);
    }
    // Function keys
    else if (key == "F1") {
      Keyboard.release(KEY_F1);
    }
    else if (key == "F2") {
      Keyboard.release(KEY_F2);
    }
    else if (key == "F3") {
      Keyboard.release(KEY_F3);
    }
    else if (key == "F4") {
      Keyboard.release(KEY_F4);
    }
    else if (key == "F5") {
      Keyboard.release(KEY_F5);
    }
    else if (key == "F6") {
      Keyboard.release(KEY_F6);
    }
    else if (key == "F7") {
      Keyboard.release(KEY_F7);
    }
    else if (key == "F8") {
      Keyboard.release(KEY_F8);
    }
    else if (key == "F9") {
      Keyboard.release(KEY_F9);
    }
    else if (key == "F10") {
      Keyboard.release(KEY_F10);
    }
    else if (key == "F11") {
      Keyboard.release(KEY_F11);
    }
    else if (key == "F12") {
      Keyboard.release(KEY_F12);
    }
    // Regular character (single char)
    else if (key.length() == 1) {
      Keyboard.release(key.charAt(0));
    }

    Serial1.println("OK:Key released: " + key);
  }
  else if (cmd == "KEY_RELEASE_ALL") {
    Keyboard.releaseAll();
    Serial1.println("OK:All keys released");
  }

  // Delay
  else if (cmd.startsWith("DELAY:")) {
    int ms = cmd.substring(6).toInt();
    if (ms > 0 && ms <= 10000) { // Max 10 seconds
      delay(ms);
      Serial1.println("OK:Delayed");
    }
  }

  // Utility commands
  else if (cmd == "PING") {
    Serial1.println("PONG");
  }
  else if (cmd == "STATUS") {
    String status = "STATUS:";
    status += jigglerEnabled ? "Jiggler=ON" : "Jiggler=OFF";
    Serial1.println(status);
  }
  else if (cmd == "LED_ON") {
    digitalWrite(LED_PIN, HIGH);
    Serial1.println("OK:LED on");
  }
  else if (cmd == "LED_OFF") {
    digitalWrite(LED_PIN, LOW);
    Serial1.println("OK:LED off");
  }

  // Unknown command
  else {
    Serial1.println("ERROR:Unknown command");
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
