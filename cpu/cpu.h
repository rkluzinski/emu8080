#ifndef INTEL_8080_H
#define INTEL_8080_H

#include <string>
#include <cstdint>

typedef uint8_t (*in_handler_t)(uint8_t);
typedef void (*out_handler_t)(uint8_t, uint8_t);

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
	friend class Intel8080Debugger;
	
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

	// for lazy flag evaluation
	struct OpCache {
		uint8_t src;
		uint8_t dst;
		uint8_t result;
	} opCache;

	// for I/O ports
	in_handler_t in_handler;
	out_handler_t out_handler;

	bool halted;

	uint16_t stack_pointer;
	uint16_t program_counter;

	uint8_t *memory;

public:
	Intel8080();
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

	void pushWord(uint16_t word);
	uint16_t popWord();

	// flag operations
	bool zeroFlag();

	// arithmetic instructions
	void increment(uint8_t &dst);
	void decrement(uint8_t &dst);
	void compare(uint8_t src);

	// branching instructions
	void jump(bool condition);
	void call(bool condition);
	void _return(bool condition);

	// complex opcodes
	void XCHG();
};

#endif