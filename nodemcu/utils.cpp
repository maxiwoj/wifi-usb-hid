#include "utils.h"

String escapeJson(String str) {
  String result = "";
  for (int i = 0; i < str.length(); i++) {
    char c = str[i];
    if (c == '\\') {
      result += "\\\\";
    } else if (c == '"') {
      result += "\\\"";;
    } else if (c == '\n') {
      result += "\\n";
    } else if (c == '\r') {
      result += "\\r";
    } else if (c == '\t') {
      result += "\\t";
    } else {
      result += c;
    }
  }
  return result;
}

