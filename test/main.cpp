#include <iostream>
#include <fstream>
#include <string>

#include "utils.h"
#include "../cpu/cpu.h"

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