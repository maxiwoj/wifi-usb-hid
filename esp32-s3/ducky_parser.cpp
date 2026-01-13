#include "ducky_parser.h"
#include "hid_handler.h"

void parseDuckyLine(String line);

void executeDuckyScript(String script) {
  Serial.println("Executing Ducky Script...");
  int lineStart = 0;
  int lineEnd = 0;

  while (lineEnd != -1) {
    lineEnd = script.indexOf('\n', lineStart);
    String line;

    if (lineEnd == -1) {
      line = script.substring(lineStart);
    } else {
      line = script.substring(lineStart, lineEnd);
      lineStart = lineEnd + 1;
    }

    line.trim();

    // Skip empty lines and comments
    if (line.length() == 0 || line.startsWith("//")) {
      continue;
    }

    parseDuckyLine(line);

    if (lineEnd == -1) break;
  }
}

void parseDuckyLine(String line) {
  line.trim();

  if (line.startsWith("DELAY ")) {
    String delayMs = line.substring(6);
    processHIDCommand("DELAY:" + delayMs);
  }
  else if (line.startsWith("STRING ")) {
    String text = line.substring(7);
    processHIDCommand("TYPE:" + text);
  }
  else if (line == "ENTER") {
    processHIDCommand("ENTER");
  }
  else if (line == "ESC") {
    processHIDCommand("ESC");
  }
  else if (line == "TAB") {
    processHIDCommand("TAB");
  }
  else if (line == "BACKSPACE") {
    processHIDCommand("BACKSPACE");
  }
  else if (line == "DELETE") {
    processHIDCommand("DELETE");
  }
  else if (line.startsWith("GUI ")) {
    String key = line.substring(4);
    if (key == "r" || key == "R") {
      processHIDCommand("GUI_R");
    } else if (key == "d" || key == "D") {
      processHIDCommand("GUI_D");
    } else if (key == "SPACE") {
      processHIDCommand("GUI_SPACE");
    } else if (key == "tab" || key == "TAB") {
      processHIDCommand("GUI_TAB");
    } else if (key == "h" || key == "H") {
      processHIDCommand("GUI_H");
    } else if (key == "w" || key == "W") {
      processHIDCommand("GUI_W");
    } else {
      processHIDCommand("GUI_" + key);
    }
  }
  else if (line == "GUI") {
    processHIDCommand("GUI");
  }
  else if (line == "ALT TAB") {
    processHIDCommand("ALT_TAB");
  }
  else if (line == "CTRL ALT DELETE") {
    processHIDCommand("CTRL_ALT_DEL");
  }
  else if (line == "CTRL ALT T") {
    processHIDCommand("CTRL_ALT_T");
  }
  else if (line.startsWith("CTRL ")) {
    String key = line.substring(5);
    processHIDCommand("CTRL_" + key);
  }
  else if (line.startsWith("ALT ")) {
    String key = line.substring(4);
    processHIDCommand("ALT_" + key);
  }
  else if (line == "UP") {
    processHIDCommand("UP");
  }
  else if (line == "DOWN") {
    processHIDCommand("DOWN");
  }
  else if (line == "LEFT") {
    processHIDCommand("LEFT");
  }
  else if (line == "RIGHT") {
    processHIDCommand("RIGHT");
  }
  else if (line == "F1" || line == "F2" || line == "F3" || line == "F4" ||
           line == "F5" || line == "F6" || line == "F7" || line == "F8" ||
           line == "F9" || line == "F10" || line == "F11" || line == "F12") {
    processHIDCommand(line);
  }
}
