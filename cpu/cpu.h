#ifndef INTEL_8080_H
#define INTEL_8080_H

#include <string>
#include <cstdint>

typedef uint8_t (*in_handler_t)(uint8_t);
typedef void (*out_handler_t)(uint8_t, uint8_t);

class Intel8080Exception : public std::exception {
	std::string _message;

public:
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

	// flags
	// https://graphics.stanford.edu/~seander/bithacks.html#ParityParallel
	bool flag_S;
	bool flag_Z;
	bool flag_A;
	bool flag_P;
	bool flag_C;

	// for I/O ports
	uint8_t (*in_callback)(uint8_t);
	void (*out_callback)(uint8_t, uint8_t);

	bool halted;
	bool interrupts_enabled;

	uint16_t stack_pointer;
	uint16_t program_counter;

	uint8_t *memory;

public:
	Intel8080(std::size_t mem_size);
	~Intel8080();

	void reset();

	void setInHandler(in_handler_t);
	void setOutHandler(out_handler_t);

	// TODO replace 
	uint8_t *getMemory() {
		return memory;
	}

	// debugging utilities
	void dump();

	std::size_t executeInstruction();
	std::size_t execute();

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