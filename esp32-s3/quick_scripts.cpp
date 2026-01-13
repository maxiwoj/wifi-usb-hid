#include "quick_scripts.h"

String getQuickScript(String scriptName, String os) {
  if (os == "Windows") {
    if (scriptName == "editor") {
      return "GUI r\nDELAY 500\nSTRING notepad\nENTER";
    } else if (scriptName == "terminal") {
      return "GUI r\nDELAY 500\nSTRING cmd\nENTER";
    } else if (scriptName == "calculator") {
      return "GUI r\nDELAY 500\nSTRING calc\nENTER";
    } else if (scriptName == "browser") {
      return "GUI r\nDELAY 500\nSTRING chrome\nENTER";
    }
  } else if (os == "MacOS") {
    if (scriptName == "editor") {
      return "GUI SPACE\nDELAY 500\nSTRING textedit\nENTER";
    } else if (scriptName == "terminal") {
      return "GUI SPACE\nDELAY 500\nSTRING terminal\nENTER";
    } else if (scriptName == "calculator") {
      return "GUI SPACE\nDELAY 500\nSTRING calculator\nENTER";
    } else if (scriptName == "browser") {
      return "GUI SPACE\nDELAY 500\nSTRING safari\nENTER";
    }
  } else if (os == "Linux") {
    if (scriptName == "editor") {
      return "CTRL ALT T\nDELAY 1000\nSTRING gedit\nENTER";
    } else if (scriptName == "terminal") {
      return "CTRL ALT T";
    } else if (scriptName == "calculator") {
      return "GUI\nDELAY 500\nSTRING calc\nENTER";
    } else if (scriptName == "browser") {
      return "GUI\nDELAY 500\nSTRING firefox\nENTER";
    }
  }
  return "";
}
