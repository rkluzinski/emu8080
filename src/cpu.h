#ifndef INTEL_8080_H
#define INTEL_8080_H

#include <cstdint>

class Intel8080 {
	Intel8080();
	~Intel8080();

	int ExecuteInstruction();

private:
	uint16_t program_counter;
	uint16_t stack_pointer;

	uint8_t *memory;
};

#endif