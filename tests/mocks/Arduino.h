#ifndef ARDUINO_MOCK_H
#define ARDUINO_MOCK_H

#include <string>
#include <cstring>
#include <iostream>

// Mock Arduino String class
class String {
private:
    std::string data;

public:
    String() : data("") {}
    String(const char* str) : data(str ? str : "") {}
    String(const std::string& str) : data(str) {}
    String(int num) : data(std::to_string(num)) {}

    // Concatenation operators
    String operator+(const String& other) const {
        return String((data + other.data).c_str());
    }

    String operator+(const char* str) const {
        return String((data + std::string(str)).c_str());
    }

    String operator+(char c) const {
        return String((data + c).c_str());
    }

    // Friend function for const char* + String concatenation
    friend String operator+(const char* lhs, const String& rhs) {
        return String((std::string(lhs) + rhs.data).c_str());
    }

    void operator+=(const String& other) {
        data += other.data;
    }

    void operator+=(const char* str) {
        data += str;
    }

    void operator+=(char c) {
        data += c;
    }

    // Comparison operators
    bool operator==(const String& other) const {
        return data == other.data;
    }

    bool operator==(const char* str) const {
        return data == std::string(str);
    }

    bool operator!=(const String& other) const {
        return data != other.data;
    }

    // String methods
    int length() const {
        return data.length();
    }

    char charAt(int index) const {
        if (index >= 0 && index < (int)data.length()) {
            return data[index];
        }
        return 0;
    }

    char operator[](int index) const {
        return charAt(index);
    }

    String substring(int start) const {
        if (start >= 0 && start < (int)data.length()) {
            return String(data.substr(start).c_str());
        }
        return String("");
    }

    String substring(int start, int end) const {
        if (start >= 0 && start < (int)data.length() && end > start) {
            return String(data.substr(start, end - start).c_str());
        }
        return String("");
    }

    int indexOf(char c) const {
        size_t pos = data.find(c);
        return (pos == std::string::npos) ? -1 : (int)pos;
    }

    int indexOf(char c, int start) const {
        size_t pos = data.find(c, start);
        return (pos == std::string::npos) ? -1 : (int)pos;
    }

    int indexOf(const String& str) const {
        size_t pos = data.find(str.data);
        return (pos == std::string::npos) ? -1 : (int)pos;
    }

    bool startsWith(const String& prefix) const {
        return data.find(prefix.data) == 0;
    }

    bool startsWith(const char* prefix) const {
        return data.find(prefix) == 0;
    }

    bool endsWith(const String& suffix) const {
        if (suffix.length() > length()) return false;
        return data.compare(data.length() - suffix.length(), suffix.length(), suffix.data) == 0;
    }

    void trim() {
        // Trim leading whitespace
        size_t start = data.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) {
            data = "";
            return;
        }

        // Trim trailing whitespace
        size_t end = data.find_last_not_of(" \t\n\r");
        data = data.substr(start, end - start + 1);
    }

    void toLowerCase() {
        for (char& c : data) {
            if (c >= 'A' && c <= 'Z') {
                c = c + ('a' - 'A');
            }
        }
    }

    void toUpperCase() {
        for (char& c : data) {
            if (c >= 'a' && c <= 'z') {
                c = c - ('a' - 'A');
            }
        }
    }

    void replace(const String& from, const String& to) {
        size_t pos = 0;
        while ((pos = data.find(from.data, pos)) != std::string::npos) {
            data.replace(pos, from.length(), to.data);
            pos += to.length();
        }
    }

    void replace(const char* from, const char* to) {
        replace(String(from), String(to));
    }

    // Conversion to C++ types
    const char* c_str() const {
        return data.c_str();
    }

    std::string toStdString() const {
        return data;
    }
};

// Mock Serial class
class SerialMock {
public:
    void begin(int baudRate) {}

    void println(const String& str) {
        std::cout << str.c_str() << std::endl;
    }

    void println(const char* str) {
        std::cout << str << std::endl;
    }

    void println(int num) {
        std::cout << num << std::endl;
    }

    void print(const String& str) {
        std::cout << str.c_str();
    }

    void print(const char* str) {
        std::cout << str;
    }

    void print(int num) {
        std::cout << num;
    }
};

// Global Serial object
extern SerialMock Serial;

#endif // ARDUINO_MOCK_H
