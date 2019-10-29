#include <iostream>
#include <fstream>
#include <string>

#include "cpu.h"

int main(int argc, char **argv) {
	Intel8080 cpu;
	std::cout << "sizeof(Intel8080) = " << sizeof(cpu) << std::endl;
	
	cpu.register_B() = 5;
	std::cout << "register_B = " << (int) cpu.register_B() << std::endl;

	return 0;
}
