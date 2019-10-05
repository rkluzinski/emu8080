#include <iostream>
#include <fstream>
#include <string>

#include "cpu.h"

bool load_rom(std::string path, uint8_t *memory) {
	std::ifstream rom(path, std::ios::in | std::ios::binary);

	if (!rom) {
        std::cerr << "Failed to open: " << path << std::endl;
		return false;
	}

	rom.seekg(0, rom.end);
	size_t length = rom.tellg();
	rom.seekg(0, rom.beg);

	if (length > 0x10000) {
        std::cerr << "Rom exceedes 8080 memory limit" << std::endl;
        return false;
    }

	rom.read((char*)memory, length);
    return true;
}

uint8_t input_device(uint8_t port) {
	return 0;
}

void output_device(uint8_t port, uint8_t byte) {
	switch (port) {
	case 0:
		std::cout << byte;
		break;
	default:
		break;
	}
}

void run_cpu(Intel8080 *cpu) {
	//tests are loaded into rom at 0x100
	cpu->halted = false;
	cpu->program_counter = 0x100;

	while (!cpu->halted) {
		try {
			cpu->ExecuteInstruction();
		}
		catch (char* c) {
			std::cerr << "Exception: " << c << std::endl;
			break;
		}
	}
}

void run_test(std::string path, Intel8080 *cpu) {
    std::cout << "(x80806_64) Running " << path << std::endl;
	if (load_rom(path, cpu->memory + 0x100)) {
	    run_cpu(cpu);
    }
    std::cout << std::endl;
}

int main(int argc, char **argv) {
	const int MEM_SIZE = 0x10000;
	
	//initialize CPU
	Intel8080 cpu;
	cpu.memory = new uint8_t[MEM_SIZE];

	//install i/o device function pointers
	cpu.input_device = input_device;
	cpu.output_device = output_device;

	//load custom bdos routine for tests
    std::cout << "(x80806_64) Loading roms/BDOS" << std::endl;
	if (!load_rom("roms/BDOS", cpu.memory)) {
        delete cpu.memory;
        return 1;
    }

    run_test("coms/HELLO.COM", &cpu);
    run_test("coms/TST8080.COM", &cpu);
    run_test("coms/8080PRE.COM", &cpu);
    run_test("coms/8080EXER.COM", &cpu);

	delete cpu.memory;
	return 0;
}
