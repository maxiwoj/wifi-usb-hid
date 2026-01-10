#include "../catch2.hpp"
#include "../mocks/Arduino.h"

// Include the actual implementation
#include "../../nodemcu/utils.cpp"

TEST_CASE("escapeJson handles empty string", "[utils]") {
    String input = "";
    String result = escapeJson(input);
    REQUIRE(result == "");
}

TEST_CASE("escapeJson handles plain text", "[utils]") {
    String input = "Hello World";
    String result = escapeJson(input);
    REQUIRE(result == "Hello World");
}

TEST_CASE("escapeJson escapes backslash", "[utils]") {
    String input = "path\\to\\file";
    String result = escapeJson(input);
    REQUIRE(result == "path\\\\to\\\\file");
}

TEST_CASE("escapeJson escapes double quotes", "[utils]") {
    String input = "He said \"Hello\"";
    String result = escapeJson(input);
    REQUIRE(result == "He said \\\"Hello\\\"");
}

TEST_CASE("escapeJson escapes newline", "[utils]") {
    String input = "Line 1\nLine 2";
    String result = escapeJson(input);
    REQUIRE(result == "Line 1\\nLine 2");
}

TEST_CASE("escapeJson escapes carriage return", "[utils]") {
    String input = "Line 1\rLine 2";
    String result = escapeJson(input);
    REQUIRE(result == "Line 1\\rLine 2");
}

TEST_CASE("escapeJson escapes tab", "[utils]") {
    String input = "Column1\tColumn2";
    String result = escapeJson(input);
    REQUIRE(result == "Column1\\tColumn2");
}

TEST_CASE("escapeJson handles multiple escape characters", "[utils]") {
    String input = "{\n\t\"name\": \"test\\file\",\n\t\"value\": \"data\"\n}";
    String result = escapeJson(input);
    REQUIRE(result == "{\\n\\t\\\"name\\\": \\\"test\\\\file\\\",\\n\\t\\\"value\\\": \\\"data\\\"\\n}");
}

TEST_CASE("escapeJson handles mixed content", "[utils]") {
    String input = "Normal text with \"quotes\" and\nnewlines\tand tabs\\and backslashes";
    String result = escapeJson(input);
    REQUIRE(result == "Normal text with \\\"quotes\\\" and\\nnewlines\\tand tabs\\\\and backslashes");
}

TEST_CASE("escapeJson handles special characters not requiring escape", "[utils]") {
    String input = "Special chars: !@#$%^&*()_+-=[]{}|;':,.<>?/~`";
    String result = escapeJson(input);
    // Only backslash and quotes should be escaped from this set
    REQUIRE(result == "Special chars: !@#$%^&*()_+-=[]{}|;':,.<>?/~`");
}
