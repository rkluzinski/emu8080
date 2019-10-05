#ifndef CPU_H
#define CPU_H

#include <iostream>
#include <cstdint>

struct Intel8080 {
	uint16_t program_counter = 0;
	uint16_t stack_pointer;
	bool interrupts_enabled = true;

	//pointer to RAM
	uint8_t *memory = NULL;

	//Input/Output device functions
	uint8_t(*input_device)(uint8_t port) = NULL;
	void(*output_device)(uint8_t port, uint8_t byte) = NULL;

	//cpu state
	bool halted = false;

	//CPU execution cycle
	void ExecuteInstruction();

	void UnimplementedInstruction();

private:
	union {
		struct {
			struct {
				uint8_t	C : 1;	//carry
				uint8_t : 1;
				uint8_t P : 1;	//parity
				uint8_t : 1;
				uint8_t A : 1;	//auxiliary carry
				uint8_t : 1;
				uint8_t	Z : 1;	//zero
				uint8_t S : 1;	//sign
			};
			uint8_t register_A;
		};
		uint16_t register_PSW;
	};
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

	uint16_t jump_target;

	//functions
	uint8_t getNextByte();
	uint16_t getNextWord();
	
	void pushWord(uint16_t);
	uint16_t popWord();

	//8bit arithmetic operations
	uint8_t incrementOp(uint8_t);
	uint8_t decrementOp(uint8_t);

	uint8_t addOp(uint8_t);
	uint8_t addcOp(uint8_t);
	uint8_t subOp(uint8_t);
	uint8_t subbOp(uint8_t);
	uint8_t andOp(uint8_t);
	uint8_t xorOp(uint8_t);
	uint8_t orOp(uint8_t);
	void compareOp(uint8_t);

	//16bit arithmetic operations
	uint16_t doubleAddOp(uint16_t);
};

#endif