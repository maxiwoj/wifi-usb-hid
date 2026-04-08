# WiFi USB HID Tests

This directory contains the comprehensive test suite for the WiFi USB HID project, including both backend (C++) and frontend (JavaScript) tests.

## Overview

The test suite includes:

### C++ Unit Tests (Catch2)
- **utils.cpp** - JSON escaping functionality
- **quick_scripts.cpp** - OS-specific quick script generation
- **ducky_parser.cpp** - DuckyScript parsing and execution logic
- **littlefs_manager.cpp** - Filename sanitization and script file management
- **eeprom_helpers.cpp** - EEPROM read/write operations

### JavaScript/UI Tests (Jest)
- **script.js** - Frontend functionality testing
  - Activity logging
  - Command sending
  - Mouse jiggler state management
  - Text typing functions
  - Mouse movement calculations
  - Command formatting

## Running Tests Locally

### C++ Tests

**Prerequisites:**
- C++ compiler (g++ or clang++)
- Make

**Build and Run:**

```bash
cd tests
make test
```

**Other Make Targets:**
- `make` or `make all` - Build the test executable
- `make test` - Build and run tests
- `make test-verbose` - Run tests with verbose output
- `make clean` - Remove build artifacts
- `make rebuild` - Clean and rebuild everything

### JavaScript Tests

**Prerequisites:**
- Node.js (v20 or higher)
- npm

**Install Dependencies:**

```bash
npm install
```

**Run Tests:**

```bash
npm test
```

**Other npm Scripts:**
- `npm test` - Run all JavaScript tests
- `npm run test:watch` - Run tests in watch mode (re-runs on file changes)
- `npm run test:coverage` - Run tests with coverage report

## Test Structure

```
tests/
├── catch2.hpp                    # Catch2 testing framework (single header)
├── mocks/                        # Mock implementations for Arduino libraries
│   ├── Arduino.h                 # Mock Arduino String class and Serial
│   ├── Arduino.cpp               # Arduino mock implementation
│   ├── EEPROM.h                  # Mock EEPROM memory
│   ├── EEPROM.cpp                # EEPROM mock implementation
│   ├── pro_micro_mock.h          # Mock for Pro Micro communication
│   └── pro_micro_mock.cpp        # Pro Micro mock implementation
├── unit/                         # C++ unit test files
│   ├── test_utils.cpp            # Tests for utils.cpp
│   ├── test_quick_scripts.cpp    # Tests for quick_scripts.cpp
│   ├── test_ducky_parser.cpp     # Tests for ducky_parser.cpp
│   ├── test_littlefs_manager.cpp # Tests for littlefs_manager.cpp
│   └── test_eeprom_helpers.cpp   # Tests for eeprom_helpers.cpp
├── ui/                           # JavaScript/UI test files
│   └── script.test.js            # Tests for script.js frontend code
├── test_main.cpp                 # C++ test runner main file
├── Makefile                      # C++ build configuration
└── README.md                     # This file
```

## Continuous Integration

Tests are automatically run on every pull request to the main branch using GitHub Actions. The workflow includes two parallel jobs:

1. **C++ Unit Tests** - Builds and runs all C++ tests using g++ and Make
2. **JavaScript/UI Tests** - Runs Jest tests for frontend functionality

See `.github/workflows/tests.yml` for the complete workflow configuration.

## Writing New Tests

### C++ Tests

To add new C++ tests:

1. Create a new test file in `unit/` directory (e.g., `test_newfeature.cpp`)
2. Include the necessary headers:
   ```cpp
   #include "../catch2.hpp"
   #include "../mocks/Arduino.h"
   // Include your source file
   #include "../../nodemcu/yourfile.cpp"
   ```
3. Write your test cases using Catch2 syntax:
   ```cpp
   TEST_CASE("Description of test", "[tag]") {
       // Your test code
       REQUIRE(expected == actual);
   }
   ```
4. Add the new test file to the `TEST_SOURCES` variable in `Makefile`
5. Run `make test` to verify your tests pass

### JavaScript Tests

To add new JavaScript tests:

1. Create or update test files in `ui/` directory (e.g., `newfeature.test.js`)
2. Write test cases using Jest syntax:
   ```javascript
   describe('Feature name', () => {
     test('should do something', () => {
       expect(actual).toBe(expected);
     });
   });
   ```
3. Run `npm test` to verify your tests pass

## Test Coverage

### C++ Unit Tests

- ✅ **utils.cpp** - JSON escaping (10 test cases)
- ✅ **quick_scripts.cpp** - OS-specific quick script generation (17 test cases)
- ✅ **ducky_parser.cpp** - DuckyScript parsing and execution (36 test cases)
- ✅ **littlefs_manager.cpp** - Filename sanitization (17 test cases)
- ✅ **eeprom_helpers.cpp** - EEPROM read/write operations (10 test cases)

**C++ Total: 89 test cases, 138 assertions**

### JavaScript/UI Tests

- ✅ **Activity logging** - Log entry management (3 test cases)
- ✅ **Command sending** - API command formatting (2 test cases)
- ✅ **Mouse jiggler** - State management and API calls (4 test cases)
- ✅ **Text typing** - Type and TypeLn commands (3 test cases)
- ✅ **Mouse movement** - Delta calculations with sensitivity (6 test cases)
- ✅ **Command formatting** - Protocol formatting (2 test cases)

**JavaScript Total: 20 test cases**

### Overall Test Coverage

**Combined Total: 109 test cases, 158 assertions**

## Mocking Arduino Libraries

Since this is an embedded Arduino project, we provide mock implementations of Arduino-specific functionality:

- **String class** - Simulates Arduino String with std::string backend (supports concatenation, substring, indexOf, replace, etc.)
- **Serial object** - Mock serial communication (prints to stdout)
- **EEPROM** - Mock EEPROM memory with read/write operations
- **sendCommandToProMicro()** - Captures commands for verification in tests

This allows us to test the business logic without requiring actual hardware.
