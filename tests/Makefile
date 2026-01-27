# Makefile for WiFi USB HID Tests

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I. -Imocks
LDFLAGS =

# Test executable
TEST_EXEC = run_tests

# Source files
TEST_SOURCES = test_main.cpp \
               unit/test_utils.cpp \
               unit/test_quick_scripts.cpp \
               unit/test_ducky_parser.cpp \
               unit/test_littlefs_manager.cpp \
               unit/test_eeprom_helpers.cpp

# Object files
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)

# Default target
all: $(TEST_EXEC)

# Build test executable
$(TEST_EXEC): $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile test files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run tests
test: $(TEST_EXEC)
	./$(TEST_EXEC)

# Run tests with verbose output
test-verbose: $(TEST_EXEC)
	./$(TEST_EXEC) -s

# Clean build artifacts
clean:
	rm -f $(TEST_EXEC) $(TEST_OBJECTS)

# Rebuild everything
rebuild: clean all

.PHONY: all test test-verbose clean rebuild
