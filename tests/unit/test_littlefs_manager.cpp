#include "../catch2.hpp"
#include "../mocks/Arduino.h"

// Mock config.h constants
#define MAX_SCRIPT_NAME_LEN 30

// Include only the testable functions from littlefs_manager
String getScriptFilename(String name) {
  // Sanitize filename - remove invalid characters
  String filename = name;
  filename.replace(" ", "_");
  filename.replace("/", "_");
  filename.replace("\\", "_");
  filename.replace("..", "_");

  // Truncate if too long
  if (filename.length() > MAX_SCRIPT_NAME_LEN) {
    filename = filename.substring(0, MAX_SCRIPT_NAME_LEN);
  }

  return String("/scripts_") + filename + ".txt";
}

String getScriptNameFromFilename(String filename) {
  // Remove "/scripts_" prefix and ".txt" suffix
  if (filename.startsWith("/scripts_") && filename.endsWith(".txt")) {
    String name = filename.substring(9); // Remove "/scripts_"
    name = name.substring(0, name.length() - 4); // Remove ".txt"

    // Convert underscores back to spaces
    name.replace("_", " ");
    return name;
  }
  return "";
}

TEST_CASE("getScriptFilename sanitizes spaces", "[littlefs_manager]") {
    String result = getScriptFilename("my script");
    REQUIRE(result == "/scripts_my_script.txt");
}

TEST_CASE("getScriptFilename sanitizes forward slashes", "[littlefs_manager]") {
    String result = getScriptFilename("path/to/script");
    REQUIRE(result == "/scripts_path_to_script.txt");
}

TEST_CASE("getScriptFilename sanitizes backslashes", "[littlefs_manager]") {
    String result = getScriptFilename("path\\to\\script");
    REQUIRE(result == "/scripts_path_to_script.txt");
}

TEST_CASE("getScriptFilename sanitizes double dots", "[littlefs_manager]") {
    String result = getScriptFilename("../../../etc/passwd");
    REQUIRE(result == "/scripts_______etc_passwd.txt");
}

TEST_CASE("getScriptFilename handles simple names", "[littlefs_manager]") {
    String result = getScriptFilename("test");
    REQUIRE(result == "/scripts_test.txt");
}

TEST_CASE("getScriptFilename truncates long names", "[littlefs_manager]") {
    String longName = "this_is_a_very_long_script_name_that_exceeds_the_maximum_length";
    String result = getScriptFilename(longName);

    // Should be truncated to MAX_SCRIPT_NAME_LEN characters
    REQUIRE(result.length() == 9 + MAX_SCRIPT_NAME_LEN + 4); // "/scripts_" + name + ".txt"
    REQUIRE(result.startsWith("/scripts_"));
    REQUIRE(result.endsWith(".txt"));
}

TEST_CASE("getScriptFilename handles empty string", "[littlefs_manager]") {
    String result = getScriptFilename("");
    REQUIRE(result == "/scripts_.txt");
}

TEST_CASE("getScriptFilename handles multiple problematic characters", "[littlefs_manager]") {
    String result = getScriptFilename("my script/../with/bad\\chars");
    REQUIRE(result == "/scripts_my_script___with_bad_chars.txt");
}

TEST_CASE("getScriptFilename handles special characters", "[littlefs_manager]") {
    String result = getScriptFilename("script@#$%");
    // Only specific characters are sanitized, others are kept
    REQUIRE(result == "/scripts_script@#$%.txt");
}

TEST_CASE("getScriptNameFromFilename extracts name correctly", "[littlefs_manager]") {
    String result = getScriptNameFromFilename("/scripts_my_script.txt");
    REQUIRE(result == "my script");
}

TEST_CASE("getScriptNameFromFilename handles simple names", "[littlefs_manager]") {
    String result = getScriptNameFromFilename("/scripts_test.txt");
    REQUIRE(result == "test");
}

TEST_CASE("getScriptNameFromFilename handles names with multiple underscores", "[littlefs_manager]") {
    String result = getScriptNameFromFilename("/scripts_my_long_script_name.txt");
    REQUIRE(result == "my long script name");
}

TEST_CASE("getScriptNameFromFilename returns empty for invalid format", "[littlefs_manager]") {
    String result = getScriptNameFromFilename("invalid_filename.txt");
    REQUIRE(result == "");
}

TEST_CASE("getScriptNameFromFilename returns empty for missing prefix", "[littlefs_manager]") {
    String result = getScriptNameFromFilename("script.txt");
    REQUIRE(result == "");
}

TEST_CASE("getScriptNameFromFilename returns empty for missing suffix", "[littlefs_manager]") {
    String result = getScriptNameFromFilename("/scripts_test");
    REQUIRE(result == "");
}

TEST_CASE("getScriptFilename and getScriptNameFromFilename roundtrip", "[littlefs_manager]") {
    String original = "my test script";
    String filename = getScriptFilename(original);
    String recovered = getScriptNameFromFilename(filename);
    REQUIRE(recovered == original);
}
