#include "ducky_parser.h"
#include "pro_micro.h"

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
    sendCommandToProMicro("DELAY:" + delayMs);
  }
  else if (line.startsWith("STRING ")) {
    String text = line.substring(7);
    sendCommandToProMicro("TYPE:" + text);
  }
  else if (line == "ENTER") {
    sendCommandToProMicro("ENTER");
  }
  else if (line == "ESC") {
    sendCommandToProMicro("ESC");
  }
  else if (line == "TAB") {
    sendCommandToProMicro("TAB");
  }
  else if (line == "BACKSPACE") {
    sendCommandToProMicro("BACKSPACE");
  }
  else if (line == "DELETE") {
    sendCommandToProMicro("DELETE");
  }
  else if (line.startsWith("GUI ")) {
    String key = line.substring(4);
    if (key == "r" || key == "R") {
      sendCommandToProMicro("GUI_R");
    } else if (key == "d" || key == "D") {
      sendCommandToProMicro("GUI_D");
    } else if (key == "SPACE") {
      sendCommandToProMicro("GUI_SPACE");
    } else if (key == "tab" || key == "TAB") {
      sendCommandToProMicro("GUI_TAB");
    } else if (key == "h" || key == "H") {
      sendCommandToProMicro("GUI_H");
    } else if (key == "w" || key == "W") {
      sendCommandToProMicro("GUI_W");
    } else {
      sendCommandToProMicro("GUI_" + key);
    }
  }
  else if (line == "GUI") {
    sendCommandToProMicro("GUI");
  }
  else if (line == "ALT TAB") {
    sendCommandToProMicro("ALT_TAB");
  }
  else if (line == "CTRL ALT DELETE") {
    sendCommandToProMicro("CTRL_ALT_DEL");
  }
  else if (line == "CTRL ALT T") {
    sendCommandToProMicro("CTRL_ALT_T");
  }
  else if (line.startsWith("CTRL ")) {
    String key = line.substring(5);
    sendCommandToProMicro("CTRL_" + key);
  }
  else if (line.startsWith("ALT ")) {
    String key = line.substring(4);
    sendCommandToProMicro("ALT_" + key);
  }
  else if (line == "UP") {
    sendCommandToProMicro("UP");
  }
  else if (line == "DOWN") {
    sendCommandToProMicro("DOWN");
  }
  else if (line == "LEFT") {
    sendCommandToProMicro("LEFT");
  }
  else if (line == "RIGHT") {
    sendCommandToProMicro("RIGHT");
  }
  else if (line == "F1" || line == "F2" || line == "F3" || line == "F4" ||
           line == "F5" || line == "F6" || line == "F7" || line == "F8" ||
           line == "F9" || line == "F10" || line == "F11" || line == "F12") {
    sendCommandToProMicro(line);
  }
}
