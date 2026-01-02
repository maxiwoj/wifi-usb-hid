#include "pro_micro.h"

// Communication with Pro Micro
void sendCommandToProMicro(String cmd) {
  Serial.println(cmd);
  Serial.flush();
}
