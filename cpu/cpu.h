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
		const volatile uint16_t cval = 0x0002;
	};

	// flags
	// https://graphics.stanford.edu/~seander/bithacks.html#ParityParallel
	bool flag_S;
	bool flag_Z;
	bool flag_A;
	bool flag_P;
	bool flag_C;

	// for I/O ports
	in_handler_t in_handler;
	out_handler_t out_handler;

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

	uint8_t readByte();
	void writeByte(const uint8_t byte);

	uint16_t readWord();
	void writeWord(uint16_t word);

	void pushWord(const uint16_t word);
	uint16_t popWord();

	// arithmetic instructions
	void INC(uint8_t &dst);
	void DCR(uint8_t &dst);

	// 8-bit arithmetic
	void _add(const uint8_t src, const uint8_t carry = 0);
	void _sub(const uint8_t src, const uint8_t carry = 0);
	void _and(const uint8_t src);
	void _xor(const uint8_t src);
	void _or(const uint8_t src);
	void _compare(const uint8_t src);

	// 16-bit arithmetic
	void _dadd(const uint16_t src);

	// branching instructions
	void _jump(const bool condition);
	void _call(const bool condition);
	void _return(const bool condition);
};

#endif