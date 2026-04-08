#ifndef EEPROM_MOCK_H
#define EEPROM_MOCK_H

#include <vector>
#include <cstdint>

// Mock EEPROM class
class EEPROMClass {
private:
    std::vector<uint8_t> memory;

public:
    EEPROMClass() : memory(4096, 0) {} // 4KB default size

    uint8_t read(int addr) {
        if (addr >= 0 && addr < (int)memory.size()) {
            return memory[addr];
        }
        return 0;
    }

    void write(int addr, uint8_t value) {
        if (addr >= 0 && addr < (int)memory.size()) {
            memory[addr] = value;
        }
    }

    void clear() {
        for (size_t i = 0; i < memory.size(); i++) {
            memory[i] = 0;
        }
    }

    // Helper for testing
    void reset() {
        clear();
    }
};

// Global EEPROM object
extern EEPROMClass EEPROM;

#endif // EEPROM_MOCK_H
