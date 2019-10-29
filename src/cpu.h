#ifndef INTEL_8080_H
#define INTEL_8080_H

#include <cstdint>

struct RegisterPair {
	union {
		struct {
			// little endian specific
			uint8_t low;
			uint8_t high;
		} byte;
		uint16_t word;
	};
};

class Intel8080 {
	RegisterPair BC;
	RegisterPair DE;
	RegisterPair HL;
	RegisterPair PSW;

	uint16_t stack_pointer;
	uint16_t program_counter;

	uint8_t *memory;

public:
	Intel8080();
	~Intel8080();

	void reset();

	std::size_t executeInstruction();

	/**
	 * Registers are aliases with the access function defined below
	 * Reference members would take up an additional 8 bytes per reference
	 * These technique has no overhead because it will be optimized away.
	 */
	
	// 8-bit register aliases
	uint8_t &register_B() { return BC.byte.high; }
	uint8_t &register_C() { return BC.byte.low; }
	uint8_t &register_D() { return DE.byte.high; }
	uint8_t &register_E() { return DE.byte.low; }
	uint8_t &register_H() { return HL.byte.high; }
	uint8_t &register_L() { return HL.byte.low; }
	uint8_t &register_A() { return PSW.byte.high; }
	// TODO flags alias

	// 16-bit register aliases
	uint16_t &register_BC() { return BC.word; }
	uint16_t &register_DE() { return DE.word; }
	uint16_t &register_HL() { return HL.word; }
	// TODO psw alias

	//
};

#endif