#include <iostream>
#include <fstream>
#include <string>

#include "../cpu/cpu.h"

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

void loadBinary(std::string path, uint8_t *memory, std::size_t length) {
	std::ifstream binaryFile;
    binaryFile.open(path, std::ios::in | std::ios::binary);

    if (binaryFile.fail()) {
        throw TestException("File not found: " + path);
    }

    binaryFile.read((char *) memory, length);

    if ((std::size_t) binaryFile.gcount() == length) {
        throw TestException("Binary exceeds Intel8080 memory limit");
    }
}

void outputDevice(uint8_t port, uint8_t byte) {
    if (port != 0) {
        throw TestException("Invalid I/O port write");
    }

    std::cout << (char) byte;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "usage: test8080 [COM]" << std::endl;
    }

	constexpr std::size_t MEMORY_SIZE = 0x10000;
	Intel8080 cpu(MEMORY_SIZE);
    cpu.setOutHandler(outputDevice);
    
    try {
        loadBinary("../roms/BDOS", cpu.getMemory(), MEMORY_SIZE);
        loadBinary(argv[1], cpu.getMemory() + 0x100, MEMORY_SIZE - 0x100);
        
        std::size_t cycles = cpu.execute();
        std::cout << "\ncycles executed: " << cycles << std::endl;
    }
    catch (TestException &e) {
        std::cerr << "TestException: " << e.what() << std::endl;
    }
    catch (Intel8080Exception &e) {
        std::cerr << "Intel8080Exception: " << e.what() << std::endl;
        cpu.dump();
    }
	return 0;
}