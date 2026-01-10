#include "../catch2.hpp"
#include "../mocks/Arduino.h"

// Include the actual implementation
#include "../../nodemcu/quick_scripts.cpp"

TEST_CASE("getQuickScript returns correct Windows editor script", "[quick_scripts]") {
    String result = getQuickScript("editor", "Windows");
    REQUIRE(result == "GUI r\nDELAY 500\nSTRING notepad\nENTER");
}

TEST_CASE("getQuickScript returns correct Windows terminal script", "[quick_scripts]") {
    String result = getQuickScript("terminal", "Windows");
    REQUIRE(result == "GUI r\nDELAY 500\nSTRING cmd\nENTER");
}

TEST_CASE("getQuickScript returns correct Windows calculator script", "[quick_scripts]") {
    String result = getQuickScript("calculator", "Windows");
    REQUIRE(result == "GUI r\nDELAY 500\nSTRING calc\nENTER");
}

TEST_CASE("getQuickScript returns correct Windows browser script", "[quick_scripts]") {
    String result = getQuickScript("browser", "Windows");
    REQUIRE(result == "GUI r\nDELAY 500\nSTRING chrome\nENTER");
}

TEST_CASE("getQuickScript returns correct MacOS editor script", "[quick_scripts]") {
    String result = getQuickScript("editor", "MacOS");
    REQUIRE(result == "GUI SPACE\nDELAY 500\nSTRING textedit\nENTER");
}

TEST_CASE("getQuickScript returns correct MacOS terminal script", "[quick_scripts]") {
    String result = getQuickScript("terminal", "MacOS");
    REQUIRE(result == "GUI SPACE\nDELAY 500\nSTRING terminal\nENTER");
}

TEST_CASE("getQuickScript returns correct MacOS calculator script", "[quick_scripts]") {
    String result = getQuickScript("calculator", "MacOS");
    REQUIRE(result == "GUI SPACE\nDELAY 500\nSTRING calculator\nENTER");
}

TEST_CASE("getQuickScript returns correct MacOS browser script", "[quick_scripts]") {
    String result = getQuickScript("browser", "MacOS");
    REQUIRE(result == "GUI SPACE\nDELAY 500\nSTRING safari\nENTER");
}

TEST_CASE("getQuickScript returns correct Linux editor script", "[quick_scripts]") {
    String result = getQuickScript("editor", "Linux");
    REQUIRE(result == "CTRL ALT T\nDELAY 1000\nSTRING gedit\nENTER");
}

TEST_CASE("getQuickScript returns correct Linux terminal script", "[quick_scripts]") {
    String result = getQuickScript("terminal", "Linux");
    REQUIRE(result == "CTRL ALT T");
}

TEST_CASE("getQuickScript returns correct Linux calculator script", "[quick_scripts]") {
    String result = getQuickScript("calculator", "Linux");
    REQUIRE(result == "GUI\nDELAY 500\nSTRING calc\nENTER");
}

TEST_CASE("getQuickScript returns correct Linux browser script", "[quick_scripts]") {
    String result = getQuickScript("browser", "Linux");
    REQUIRE(result == "GUI\nDELAY 500\nSTRING firefox\nENTER");
}

TEST_CASE("getQuickScript returns empty string for unknown script", "[quick_scripts]") {
    String result = getQuickScript("unknown", "Windows");
    REQUIRE(result == "");
}

TEST_CASE("getQuickScript returns empty string for unknown OS", "[quick_scripts]") {
    String result = getQuickScript("editor", "UnknownOS");
    REQUIRE(result == "");
}

TEST_CASE("getQuickScript returns empty string for both unknown script and OS", "[quick_scripts]") {
    String result = getQuickScript("unknown", "UnknownOS");
    REQUIRE(result == "");
}

TEST_CASE("getQuickScript handles empty strings", "[quick_scripts]") {
    String result = getQuickScript("", "");
    REQUIRE(result == "");
}
