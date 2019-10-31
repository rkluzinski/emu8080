#include <iostream>
#include <fstream>
#include <string>

#include "../src/cpu.h"

int main(int argc, char **argv) {
	Intel8080 cpu;
	std::cout << "sizeof(Intel8080) = " << sizeof(cpu) << std::endl;

	return 0;
}
