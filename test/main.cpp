#include <iostream>
#include <fstream>
#include <string>

#include "../cpu/cpu.h"

// assembled test BDOS file
const std::array<uint8_t, 0x22> bdos = {
    0x76, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x02, 0xb9,
    0xca, 0x14, 0x00, 0x3e, 0x09, 0xb9, 0xca, 0x18,
    0x00, 0xc3, 0x00, 0x00, 0x7b, 0xd3, 0x00, 0xc9,
    0x1a, 0xfe, 0x24, 0xc8, 0xd3, 0x00, 0x13, 0xc3,
    0x18, 0x00
};

int main(int argc, char **argv) {
    // check arguments
    if (argc != 2) {
        std::cout << "usage: test8080 [COM]" << std::endl;
        return 1;
    }

    // open test file
    std::ifstream test(argv[1], std::ios::in | std::ios::binary);
    if (test.fail()) {
        return 1;
    }

    Intel8080 cpu;
    cpu.program_counter = 0x100;
    
    // CPU out port callback
    cpu.out = [](uint8_t port, uint8_t byte) {
        if (port == 0) {
            std::cout << byte;
        }
    };

    // load BDOS test file into RAM
    std::copy(bdos.begin(), bdos.end(), cpu.memory.begin());
	test.read((char *) cpu.memory.data() + 0x100, cpu.memory.size() - 0x100);
    
    // execute program
    auto cycles = cpu.execute();
    std::cout << std::endl << "Cycles executed: " << cycles << std::endl;
	return 0;
}