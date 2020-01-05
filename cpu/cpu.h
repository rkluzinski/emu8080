#ifndef INTEL_8080_H
#define INTEL_8080_H

#include <array>
#include <functional>
#include <string>
#include <cstdint>

class Intel8080 {
public:
	// cpu registers
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
	
	uint16_t stack_pointer = 0x0000; // TODO fix
	uint16_t program_counter = 0x100; // TODO fix

	bool flag_S;
	bool flag_Z;
	bool flag_A;
	bool flag_P;
	bool flag_C;

	// I/O port call backs
	std::function<uint8_t(uint8_t)> in;
	std::function<void(uint8_t, uint8_t)> out;

	bool halted = false;
	bool interrupts_enabled = true;

	// default 64 KB of RAM
	// TODO - better solution to creating memory, templates?
	std::array<uint8_t, 0x10000> memory;

	std::size_t executeInstruction();
	std::size_t executeFor(std::size_t);
	std::size_t execute();

	// reset the state of the CPU
	void reset();

private:
	uint8_t nextByte();
	uint16_t nextWord();

	void push(const uint16_t word);
	uint16_t pop();

	void updateZSP(const uint8_t result);
	void storeFlags();
	void loadFlags();

	// register inrrement and decrement
	uint8_t inr(uint8_t value);
	uint8_t dcr(uint8_t value);

	// 8-bit arithmetic
	void add(const uint8_t value);
	void adc(const uint8_t value);
	void sub(const uint8_t value);
	void sbb(const uint8_t value);
	void ana(const uint8_t value);
	void xra(const uint8_t value);
	void ora(const uint8_t value);
	void cmp(const uint8_t value);

	// 16-bit arithmetic
	void dad(const uint16_t src);

	// branching instructions
	void jmp(const bool condition);
	void call(const bool condition);
	void ret(const bool condition);
};

#endif