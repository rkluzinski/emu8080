#ifndef INTEL_8080_H
#define INTEL_8080_H

#include <string>
#include <cstdint>

class Intel8080Exception : public std::exception {
	std::string _message;

public:
	Intel8080Exception() : _message("") {}
	Intel8080Exception(const char *message) : _message(message) {}
	Intel8080Exception(std::string message) : _message(message) {}

	const char *what() {
		return _message.c_str();
	}
};

class Intel8080 {
	union { 
		struct {
			uint8_t register_C;
			uint8_t register_B;
		};
		uint16_t register_BC;
	};
	union {
		struct {
			uint8_t register_E;
			uint8_t register_D;
		};
		uint16_t register_DE;
	}; 
	union {
		struct {
			uint8_t register_L;
			uint8_t register_H;
		};
		uint16_t register_HL;
	}; 
	union {
		struct {
			uint8_t flags;
			uint8_t register_A;
		};
		uint16_t register_PSW;
	};

	uint16_t stack_pointer;
	uint16_t program_counter;

	uint8_t *memory;

	bool halted;

public:
	Intel8080();
	// TODO other constructors
	~Intel8080();

	void reset();

	// TODO replace 
	uint8_t *getMemory() {
		return memory;
	}

	// debugging utilities
	void dump();

	std::size_t executeInstruction();
	std::size_t execute();

private:
	// helper functions
	// opcodes
};

#endif