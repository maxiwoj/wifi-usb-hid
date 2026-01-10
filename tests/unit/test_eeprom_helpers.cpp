#include "../catch2.hpp"
#include "../mocks/Arduino.h"
#include "../mocks/EEPROM.h"

// Include the actual implementation
#include "../../nodemcu/eeprom_helpers.cpp"

TEST_CASE("writeStringToEEPROM and readStringFromEEPROM roundtrip", "[eeprom_helpers]") {
    EEPROM.reset();

    String original = "Hello World";
    writeStringToEEPROM(0, original, 50);
    String result = readStringFromEEPROM(0, 50);

    REQUIRE(result == original);
}

TEST_CASE("writeStringToEEPROM handles empty string", "[eeprom_helpers]") {
    EEPROM.reset();

    String original = "";
    writeStringToEEPROM(0, original, 50);
    String result = readStringFromEEPROM(0, 50);

    REQUIRE(result == original);
}

TEST_CASE("writeStringToEEPROM truncates long strings", "[eeprom_helpers]") {
    EEPROM.reset();

    String original = "This is a very long string that will be truncated";
    int maxLen = 20;
    writeStringToEEPROM(0, original, maxLen);
    String result = readStringFromEEPROM(0, maxLen);

    // Should be truncated to maxLen characters
    REQUIRE(result.length() <= maxLen);
    REQUIRE(result == original.substring(0, maxLen));
}

TEST_CASE("writeStringToEEPROM null terminates", "[eeprom_helpers]") {
    EEPROM.reset();

    String original = "Test";
    writeStringToEEPROM(0, original, 50);

    // Check that byte after string is null
    REQUIRE(EEPROM.read(4) == 0);
}

TEST_CASE("readStringFromEEPROM stops at null terminator", "[eeprom_helpers]") {
    EEPROM.reset();

    // Manually write a string with extra data after null
    EEPROM.write(0, 'H');
    EEPROM.write(1, 'i');
    EEPROM.write(2, 0); // Null terminator
    EEPROM.write(3, 'X');
    EEPROM.write(4, 'X');

    String result = readStringFromEEPROM(0, 50);
    REQUIRE(result == "Hi");
}

TEST_CASE("writeStringToEEPROM handles special characters", "[eeprom_helpers]") {
    EEPROM.reset();

    String original = "Test@#$%^&*()!";
    writeStringToEEPROM(0, original, 50);
    String result = readStringFromEEPROM(0, 50);

    REQUIRE(result == original);
}

TEST_CASE("Multiple strings at different addresses", "[eeprom_helpers]") {
    EEPROM.reset();

    String str1 = "First";
    String str2 = "Second";

    writeStringToEEPROM(0, str1, 20);
    writeStringToEEPROM(100, str2, 20);

    String result1 = readStringFromEEPROM(0, 20);
    String result2 = readStringFromEEPROM(100, 20);

    REQUIRE(result1 == str1);
    REQUIRE(result2 == str2);
}

TEST_CASE("writeStringToEEPROM with exact maxLen", "[eeprom_helpers]") {
    EEPROM.reset();

    String original = "12345";
    writeStringToEEPROM(0, original, 5); // Exact length, no room for null
    String result = readStringFromEEPROM(0, 5);

    REQUIRE(result == original);
}

TEST_CASE("readStringFromEEPROM from uninitialized memory", "[eeprom_helpers]") {
    EEPROM.reset(); // All zeros

    String result = readStringFromEEPROM(0, 50);
    REQUIRE(result == "");
}

TEST_CASE("writeStringToEEPROM overwrites previous data", "[eeprom_helpers]") {
    EEPROM.reset();

    // Write first string
    writeStringToEEPROM(0, "LongString", 50);

    // Write shorter string at same location
    writeStringToEEPROM(0, "Short", 50);

    String result = readStringFromEEPROM(0, 50);
    REQUIRE(result == "Short");
}
