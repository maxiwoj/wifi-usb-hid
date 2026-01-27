#ifndef PRO_MICRO_MOCK_H
#define PRO_MICRO_MOCK_H

#include "Arduino.h"
#include <vector>
#include <string>

// Store commands sent to Pro Micro for testing
extern std::vector<std::string> sentCommands;

void sendCommandToProMicro(String cmd);
void clearSentCommands();

#endif // PRO_MICRO_MOCK_H
