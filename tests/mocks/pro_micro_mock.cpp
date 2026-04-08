#include "pro_micro_mock.h"

std::vector<std::string> sentCommands;

void sendCommandToProMicro(String cmd) {
    sentCommands.push_back(cmd.toStdString());
}

void clearSentCommands() {
    sentCommands.clear();
}
