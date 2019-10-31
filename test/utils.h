#ifndef INTEGRATION_TEST_UTILS_H
#define INTEGRATION_TEST_UTILS_H

#include <string>

class TestException : std::exception {
    std::string _message;

public:
    TestException(): _message("") {}
    TestException(const char *message): _message(message) {}
    TestException(std::string message): _message(message) {}

    const char *what() {
        return _message.c_str();
    }
};

void loadBinary(std::string path, uint8_t *memory, std::size_t length);

void outputDevice(uint8_t port, uint8_t byte);

#endif