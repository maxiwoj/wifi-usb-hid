#include "../catch2.hpp"
#include "../mocks/Arduino.h"
#include "../mocks/pro_micro_mock.h"

// Include the actual implementation
#include "../../nodemcu/ducky_parser.cpp"

TEST_CASE("parseDuckyLine handles DELAY command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("DELAY 1000");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "DELAY:1000");
}

TEST_CASE("parseDuckyLine handles STRING command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("STRING Hello World");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "TYPE:Hello World");
}

TEST_CASE("parseDuckyLine handles ENTER command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("ENTER");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "ENTER");
}

TEST_CASE("parseDuckyLine handles ESC command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("ESC");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "ESC");
}

TEST_CASE("parseDuckyLine handles TAB command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("TAB");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "TAB");
}

TEST_CASE("parseDuckyLine handles BACKSPACE command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("BACKSPACE");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "BACKSPACE");
}

TEST_CASE("parseDuckyLine handles DELETE command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("DELETE");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "DELETE");
}

TEST_CASE("parseDuckyLine handles GUI command alone", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("GUI");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "GUI");
}

TEST_CASE("parseDuckyLine handles GUI r command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("GUI r");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "GUI_R");
}

TEST_CASE("parseDuckyLine handles GUI R command (uppercase)", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("GUI R");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "GUI_R");
}

TEST_CASE("parseDuckyLine handles GUI d command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("GUI d");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "GUI_D");
}

TEST_CASE("parseDuckyLine handles GUI SPACE command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("GUI SPACE");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "GUI_SPACE");
}

TEST_CASE("parseDuckyLine handles GUI TAB command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("GUI TAB");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "GUI_TAB");
}

TEST_CASE("parseDuckyLine handles GUI tab command (lowercase)", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("GUI tab");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "GUI_TAB");
}

TEST_CASE("parseDuckyLine handles GUI h command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("GUI h");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "GUI_H");
}

TEST_CASE("parseDuckyLine handles GUI w command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("GUI w");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "GUI_W");
}

TEST_CASE("parseDuckyLine handles GUI with arbitrary key", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("GUI x");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "GUI_x");
}

TEST_CASE("parseDuckyLine handles ALT TAB command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("ALT TAB");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "ALT_TAB");
}

TEST_CASE("parseDuckyLine handles CTRL ALT DELETE command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("CTRL ALT DELETE");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "CTRL_ALT_DEL");
}

TEST_CASE("parseDuckyLine handles CTRL ALT T command", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("CTRL ALT T");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "CTRL_ALT_T");
}

TEST_CASE("parseDuckyLine handles CTRL with key", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("CTRL c");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "CTRL_c");
}

TEST_CASE("parseDuckyLine handles ALT with key", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("ALT f4");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "ALT_f4");
}

TEST_CASE("parseDuckyLine handles UP arrow", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("UP");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "UP");
}

TEST_CASE("parseDuckyLine handles DOWN arrow", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("DOWN");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "DOWN");
}

TEST_CASE("parseDuckyLine handles LEFT arrow", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("LEFT");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "LEFT");
}

TEST_CASE("parseDuckyLine handles RIGHT arrow", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("RIGHT");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "RIGHT");
}

TEST_CASE("parseDuckyLine handles F1 key", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("F1");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "F1");
}

TEST_CASE("parseDuckyLine handles F12 key", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("F12");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "F12");
}

TEST_CASE("executeDuckyScript handles multi-line script", "[ducky_parser]") {
    clearSentCommands();
    String script = "GUI r\nDELAY 500\nSTRING notepad\nENTER";
    executeDuckyScript(script);

    REQUIRE(sentCommands.size() == 4);
    REQUIRE(sentCommands[0] == "GUI_R");
    REQUIRE(sentCommands[1] == "DELAY:500");
    REQUIRE(sentCommands[2] == "TYPE:notepad");
    REQUIRE(sentCommands[3] == "ENTER");
}

TEST_CASE("executeDuckyScript skips empty lines", "[ducky_parser]") {
    clearSentCommands();
    String script = "ENTER\n\n\nTAB";
    executeDuckyScript(script);

    REQUIRE(sentCommands.size() == 2);
    REQUIRE(sentCommands[0] == "ENTER");
    REQUIRE(sentCommands[1] == "TAB");
}

TEST_CASE("executeDuckyScript skips comment lines", "[ducky_parser]") {
    clearSentCommands();
    String script = "ENTER\n// This is a comment\nTAB\n// Another comment";
    executeDuckyScript(script);

    REQUIRE(sentCommands.size() == 2);
    REQUIRE(sentCommands[0] == "ENTER");
    REQUIRE(sentCommands[1] == "TAB");
}

TEST_CASE("executeDuckyScript handles script without newline at end", "[ducky_parser]") {
    clearSentCommands();
    String script = "ENTER";
    executeDuckyScript(script);

    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "ENTER");
}

TEST_CASE("executeDuckyScript handles empty script", "[ducky_parser]") {
    clearSentCommands();
    String script = "";
    executeDuckyScript(script);

    REQUIRE(sentCommands.size() == 0);
}

TEST_CASE("executeDuckyScript handles script with only whitespace", "[ducky_parser]") {
    clearSentCommands();
    String script = "   \n  \n   ";
    executeDuckyScript(script);

    REQUIRE(sentCommands.size() == 0);
}

TEST_CASE("executeDuckyScript handles script with only comments", "[ducky_parser]") {
    clearSentCommands();
    String script = "// Comment 1\n// Comment 2\n// Comment 3";
    executeDuckyScript(script);

    REQUIRE(sentCommands.size() == 0);
}

TEST_CASE("parseDuckyLine handles lines with leading/trailing whitespace", "[ducky_parser]") {
    clearSentCommands();
    parseDuckyLine("  ENTER  ");
    REQUIRE(sentCommands.size() == 1);
    REQUIRE(sentCommands[0] == "ENTER");
}

TEST_CASE("executeDuckyScript handles complex automation script", "[ducky_parser]") {
    clearSentCommands();
    String script = "// Open Run dialog\n"
                    "GUI r\n"
                    "DELAY 200\n"
                    "// Type command\n"
                    "STRING cmd\n"
                    "ENTER\n"
                    "DELAY 1000\n"
                    "STRING echo Hello World\n"
                    "ENTER";
    executeDuckyScript(script);

    REQUIRE(sentCommands.size() == 7);
    REQUIRE(sentCommands[0] == "GUI_R");
    REQUIRE(sentCommands[1] == "DELAY:200");
    REQUIRE(sentCommands[2] == "TYPE:cmd");
    REQUIRE(sentCommands[3] == "ENTER");
    REQUIRE(sentCommands[4] == "DELAY:1000");
    REQUIRE(sentCommands[5] == "TYPE:echo Hello World");
    REQUIRE(sentCommands[6] == "ENTER");
}
