#include <fstream>
#include <iostream>
#include <cassert>

#include "utils.h"

void loadBinary(std::string path, uint8_t *memory, std::size_t length) {
	std::ifstream binaryFile;
    binaryFile.open(path, std::ios::in | std::ios::binary);

    if (binaryFile.fail()) {
        throw TestException("File not found: " + path);
    }

    binaryFile.read((char *) memory, length);

    if (binaryFile.gcount() == length) {
        throw TestException("Binary exceeds Intel8080 memory limit");
    }
}

void outputDevice(uint8_t port, uint8_t byte) {
    if (port != 0) {
        throw TestException("Invalid I/O port write");
    }

    std::cout << byte;
}