# WiFi USB HID Tests

This directory contains the test suite for the WiFi USB HID project.

## Overview

The test suite uses [Catch2](https://github.com/catchorg/Catch2) testing framework and includes comprehensive unit tests for:

- **utils.cpp** - JSON escaping functionality
- **quick_scripts.cpp** - OS-specific quick script generation
- **ducky_parser.cpp** - DuckyScript parsing and execution logic

## Running Tests Locally

### Prerequisites

- C++ compiler (g++ or clang++)
- Make

### Build and Run Tests

```bash
cd tests
make test
```

### Other Make Targets

- `make` or `make all` - Build the test executable
- `make test` - Build and run tests
- `make test-verbose` - Run tests with verbose output
- `make clean` - Remove build artifacts
- `make rebuild` - Clean and rebuild everything

## Test Structure

```
tests/
├── catch2.hpp              # Catch2 testing framework (single header)
├── mocks/                  # Mock implementations for Arduino libraries
│   ├── Arduino.h           # Mock Arduino String class and Serial
│   ├── Arduino.cpp         # Arduino mock implementation
│   ├── pro_micro_mock.h    # Mock for Pro Micro communication
│   └── pro_micro_mock.cpp  # Pro Micro mock implementation
├── unit/                   # Unit test files
│   ├── test_utils.cpp      # Tests for utils.cpp
│   ├── test_quick_scripts.cpp  # Tests for quick_scripts.cpp
│   └── test_ducky_parser.cpp   # Tests for ducky_parser.cpp
├── test_main.cpp           # Test runner main file
├── Makefile                # Build configuration
└── README.md               # This file
```

## Continuous Integration

Tests are automatically run on every pull request to the main branch using GitHub Actions. See `.github/workflows/tests.yml` for the workflow configuration.

## Writing New Tests

To add new tests:

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

## Test Coverage

Current test coverage:

- ✅ JSON escaping (10 test cases)
- ✅ Quick script generation for Windows/MacOS/Linux (17 test cases)
- ✅ DuckyScript parser for all commands (36 test cases)

**Total: 63 test cases, 108 assertions**

## Mocking Arduino Libraries

Since this is an embedded Arduino project, we provide mock implementations of Arduino-specific functionality:

- **String class** - Simulates Arduino String with std::string backend
- **Serial object** - Mock serial communication (prints to stdout)
- **sendCommandToProMicro()** - Captures commands for verification in tests

This allows us to test the business logic without requiring actual hardware.
