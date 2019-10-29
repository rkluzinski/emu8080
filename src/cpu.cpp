#include "cpu.h"

//https://stackoverflow.com/questions/21617970
uint8_t parity(uint8_t byte) {
	byte ^= byte >> 4;
	byte ^= byte >> 2;
	byte ^= byte >> 1;
	return (~byte) & 1;
}

void Intel8080::UnimplementedInstruction() {
	program_counter--; //backup program counter
	std::cout << "PC = 0x" << std::hex << program_counter << std::endl;
	throw "Unimplemented Instruction.";
}

uint8_t Intel8080::getNextByte() {
	return memory[program_counter++];
}

uint16_t Intel8080::getNextWord() {
	uint16_t word = memory[program_counter] | (memory[program_counter + 1]) << 8;
	program_counter += 2;
	return word;
}

void Intel8080::pushWord(uint16_t word) {
	memory[stack_pointer - 2] = (uint8_t)(word);
	memory[stack_pointer - 1] = (uint8_t)(word >> 8);
	stack_pointer -= 2;
}

uint16_t Intel8080::popWord() {
	stack_pointer += 2;
	return memory[stack_pointer - 2] | (memory[stack_pointer - 1]) << 8;
}

uint8_t Intel8080::getCarryFlag() {
	return alu_result > 0xff;
}

uint8_t Intel8080::getAuxCarryFlag() {

}

uint8_t Intel8080::getSignFlag() {

}

uint8_t Intel8080::getZeroFlag() {

}

uint8_t Intel8080::getParityFlag() {

}

uint8_t Intel8080::addcOp(uint8_t r) {
	uint16_t result = register_A + r + C;
	C = result > 0xff;
	S = result >> 7;
	Z = (result & 0xff) == 0;
	P = parity((uint8_t)result);
	A = (register_A & 0xf) + (r & 0xf) > 0xf;
	return (uint8_t)result;
}

uint8_t Intel8080::subbOp(uint8_t r) {
	uint16_t result = register_A - r - C;
	C = result > 0xff;
	S = result >> 7;
	Z = (result & 0xff) == 0;
	P = parity((uint8_t)result);
	A = (register_A & 0xf) + (r & 0xf) > 0xf;
	return (uint8_t)result;
}

void Intel8080::compareOp(uint8_t r) {
	uint16_t result = register_A - r;
	C = result > 0xff;
	S = result >> 7;
	Z = (result & 0xff) == 0;
	P = parity((uint8_t)result);
	A = (register_A & 0xf) + (r & 0xf) > 0xf;
}

void Intel8080::op_inc(uint8_t &value) {
	++value;
	C = value < 1;
	S = value >> 7;
	Z = value == 0;
	P = parity(value);
	A = (value & 0xF) < 1;
}

void Intel8080::op_dec(uint8_t &value) {
	--value;
	C = value < 1;
	S = value >> 7;
	Z = value == 0;
	P = parity(value);
	A = (value & 0xF) < 1;
}

void Intel8080::op_add(const uint8_t value) {
	register_A += value;
	C = register_A < value;
	S = register_A >> 7;
	Z = register_A == 0;
	P = parity(register_A);
	A = (register_A & 0xF) < (value & 0xF); 
}

void Intel8080::op_add(const uint16_t value) {
	register_HL += value;
	C = register_HL < value;
}

void Intel8080::op_sub(const uint8_t value) {
	C = register_A < value;
	register_A -= value;
	S = register_A >> 7;
	Z = register_A == 0;
	P = parity(register_A);
	A = (register_A & 0xF) < (value & 0xF);
}

void Intel8080::op_cmp(const uint8_t value) {
	// TODO implement
}

void Intel8080::op_xor(const uint8_t value) {
	register_A ^= value;
	C = 0;
	S = register_A >> 7;
	Z = register_A == 0;
	P = parity(register_A);
}

void Intel8080::op_and(const uint8_t value) {
	register_A &= value;
	C = 0;
	S = register_A >> 7;
	Z = register_A == 0;
	P = parity(register_A);
}

void Intel8080::op_or(const uint8_t value) {
	register_A |= value;
	C = 0;
	S = register_A >> 7;
	Z = register_A == 0;
	P = parity(register_A);
}

void Intel8080::op_addc(const uint8_t value) {

}

void Intel8080::op_subb(const uint8_t value) {

}

void Intel8080::ExecuteInstruction() {
	//fetch instruction
	uint8_t instruction = memory[program_counter++];

	//execute instructon
	switch (instruction) {
	default: UnimplementedInstruction(); break;

		//NOP
	case 0x00:
	case 0x08:
	case 0x10:
	case 0x18:
	case 0x20:
	case 0x28:
	case 0x30:
	case 0x38: break;

		//LXI rp, d16
	case 0x01: register_BC = getNextWord(); break;
	case 0x11: register_DE = getNextWord(); break;
	case 0x21: register_HL = getNextWord(); break;
	case 0x31: stack_pointer = getNextWord(); break;

		//STAX rp
	case 0x02: memory[register_BC] = register_A; break;
	case 0x12: memory[register_DE] = register_A; break;

		//LDAX rp
	case 0x0a: register_A = memory[register_BC]; break;
	case 0x1a: register_A = memory[register_DE]; break;

		//INX rp
	case 0x03: register_BC++; break;
	case 0x13: register_DE++; break;
	case 0x23: register_HL++; break;
	case 0x33: stack_pointer++; break;

		//DCX rp
	case 0x0b: register_BC--; break;
	case 0x1b: register_DE--; break;
	case 0x2b: register_HL--; break;
	case 0x3b: stack_pointer--; break;

		//INR r
	case 0x04: op_inc(register_B); break;
	case 0x0c: op_inc(register_C); break;
	case 0x14: op_inc(register_D); break;
	case 0x1c: op_inc(register_E); break;
	case 0x24: op_inc(register_H); break;
	case 0x2c: op_inc(register_L); break;
	case 0x34: op_inc(memory[register_HL]); break;
	case 0x3c: op_inc(register_A);  break;

		//DCR r
	case 0x05: op_dec(register_B); break;
	case 0x0d: op_dec(register_C); break;
	case 0x15: op_dec(register_D); break;
	case 0x1d: op_dec(register_E); break;
	case 0x25: op_dec(register_H); break;
	case 0x2d: op_dec(register_L); break;
	case 0x35: op_dec(memory[register_HL]); break;
	case 0x3d: op_dec(register_A); break;

		//MVI r, d8
	case 0x06: register_B = getNextByte(); break;
	case 0x0e: register_C = getNextByte(); break;
	case 0x16: register_D = getNextByte(); break;
	case 0x1e: register_E = getNextByte(); break;
	case 0x26: register_H = getNextByte(); break;
	case 0x2e: register_L = getNextByte(); break;
	case 0x36: memory[register_HL] = getNextByte(); break;
	case 0x3e: register_A = getNextByte(); break;

		//DAD rp
	case 0x09: op_add(register_BC); break;
	case 0x19: op_add(register_DE); break;
	case 0x29: op_add(register_HL); break;
	case 0x39: op_add(stack_pointer); break;

		//RLC
	case 0x07:
		C = (register_A & 0x80) == 0x80;
		register_A = (register_A << 1) | C;
		break;

		//RRC
	case 0x0f:
		C = register_A & 0x1;
		register_A = (register_A >> 1) | (C << 7);
		break;

		//RAL
	case 0x17: {
		uint16_t result = register_A << 1;
		register_A = result | C;
		C = (result & 0x100) == 0x100;
		break;
	}

		//RAR
	case 0x1f: {
		uint16_t result = register_A | (C << 8);
		C = (register_A & 0x1) == 0x1;
		register_A = (uint8_t)(result >> 1);
		break;
	}

			   //SHLD d16
	case 0x22: {
		uint16_t address = getNextWord();
		memory[address] = register_L;
		memory[address + 1] = register_H;
		break;
	}

			   //LHLD d16
	case 0x2a: {
		uint16_t address = getNextWord();
		register_L = memory[address];
		register_H = memory[address + 1];
		break;
	}

			   //DAA
	case 0x27: {
		if ((register_A & 0xf) > 9 || A == 1) {
			uint8_t result = register_A + 6;
			A = result > 0xf;
			register_A = result;
		}
		if ((register_A & 0xf0) > (9 << 4) || C == 1) {
			uint16_t result = register_A + (6 << 4);
			C = result > 0xff;
			register_A = result & 0xff;
		}
		break;
	}

	case 0x2f: register_A = ~register_A; break; //CMA

	case 0x32: memory[getNextWord()] = register_A; break; //STA d16
	case 0x3a: register_A = memory[getNextWord()]; break; //LDA d16

	case 0x37: C = 1; break; //STC
	case 0x3f: C = ~C; break; //CMC

	//MOV r, r
	case 0x40: register_B = register_B; break;
	case 0x41: register_B = register_C; break;
	case 0x42: register_B = register_D; break;
	case 0x43: register_B = register_E; break;
	case 0x44: register_B = register_H; break;
	case 0x45: register_B = register_L; break;
	case 0x46: register_B = memory[register_HL]; break;
	case 0x47: register_B = register_A; break;

	case 0x48: register_C = register_B; break;
	case 0x49: register_C = register_C; break;
	case 0x4a: register_C = register_D; break;
	case 0x4b: register_C = register_E; break;
	case 0x4c: register_C = register_H; break;
	case 0x4d: register_C = register_L; break;
	case 0x4e: register_C = memory[register_HL]; break;
	case 0x4f: register_C = register_A; break;

	case 0x50: register_D = register_B; break;
	case 0x51: register_D = register_C; break;
	case 0x52: register_D = register_D; break;
	case 0x53: register_D = register_E; break;
	case 0x54: register_D = register_H; break;
	case 0x55: register_D = register_L; break;
	case 0x56: register_D = memory[register_HL]; break;
	case 0x57: register_D = register_A; break;

	case 0x58: register_E = register_B; break;
	case 0x59: register_E = register_C; break;
	case 0x5a: register_E = register_D; break;
	case 0x5b: register_E = register_E; break;
	case 0x5c: register_E = register_H; break;
	case 0x5d: register_E = register_L; break;
	case 0x5e: register_E = memory[register_HL]; break;
	case 0x5f: register_E = register_A; break;

	case 0x60: register_H = register_B; break;
	case 0x61: register_H = register_C; break;
	case 0x62: register_H = register_D; break;
	case 0x63: register_H = register_E; break;
	case 0x64: register_H = register_H; break;
	case 0x65: register_H = register_L; break;
	case 0x66: register_H = memory[register_HL]; break;
	case 0x67: register_H = register_A; break;

	case 0x68: register_L = register_B; break;
	case 0x69: register_L = register_C; break;
	case 0x6a: register_L = register_D; break;
	case 0x6b: register_L = register_E; break;
	case 0x6c: register_L = register_H; break;
	case 0x6d: register_L = register_L; break;
	case 0x6e: register_L = memory[register_HL]; break;
	case 0x6f: register_L = register_A; break;

	case 0x70: memory[register_HL] = register_B; break;
	case 0x71: memory[register_HL] = register_C; break;
	case 0x72: memory[register_HL] = register_D; break;
	case 0x73: memory[register_HL] = register_E; break;
	case 0x74: memory[register_HL] = register_H; break;
	case 0x75: memory[register_HL] = register_L; break;
	case 0x77: memory[register_HL] = register_A; break;

	case 0x78: register_A = register_B; break;
	case 0x79: register_A = register_C; break;
	case 0x7a: register_A = register_D; break;
	case 0x7b: register_A = register_E; break;
	case 0x7c: register_A = register_H; break;
	case 0x7d: register_A = register_L; break;
	case 0x7e: register_A = memory[register_HL]; break;
	case 0x7f: register_A = register_A; break;

		//HLT
	case 0x76: halted = true; break;

		//ADD r
	case 0x80: op_add(register_B); break;
	case 0x81: op_add(register_C); break;
	case 0x82: op_add(register_D); break;
	case 0x83: op_add(register_E); break;
	case 0x84: op_add(register_H); break;
	case 0x85: op_add(register_L); break;
	case 0x86: op_add(memory[register_HL]); break;
	case 0x87: op_add(register_A); break;

		//ADC r
	case 0x88: register_A = addcOp(register_B); break;
	case 0x89: register_A = addcOp(register_C); break;
	case 0x8a: register_A = addcOp(register_D); break;
	case 0x8b: register_A = addcOp(register_E); break;
	case 0x8c: register_A = addcOp(register_H); break;
	case 0x8d: register_A = addcOp(register_L); break;
	case 0x8e: register_A = addcOp(memory[register_HL]); break;
	case 0x8f: register_A = addcOp(register_A); break;

		//SUB r
	case 0x90: op_sub(register_B); break;
	case 0x91: op_sub(register_C); break;
	case 0x92: op_sub(register_D); break;
	case 0x93: op_sub(register_E); break;
	case 0x94: op_sub(register_H); break;
	case 0x95: op_sub(register_L); break;
	case 0x96: op_sub(memory[register_HL]); break;
	case 0x97: op_sub(register_A); break;

		//SBB r
	case 0x98: register_A = subbOp(register_B); break;
	case 0x99: register_A = subbOp(register_C); break;
	case 0x9a: register_A = subbOp(register_D); break;
	case 0x9b: register_A = subbOp(register_E); break;
	case 0x9c: register_A = subbOp(register_H); break;
	case 0x9d: register_A = subbOp(register_L); break;
	case 0x9e: register_A = subbOp(memory[register_HL]); break;
	case 0x9f: register_A = subbOp(register_A); break;

		//ANA r
	case 0xa0: op_and(register_B); break;
	case 0xa1: op_and(register_C); break;
	case 0xa2: op_and(register_D); break;
	case 0xa3: op_and(register_E); break;
	case 0xa4: op_and(register_H); break;
	case 0xa5: op_and(register_L); break;
	case 0xa6: op_and(memory[register_HL]); break;
	case 0xa7: op_and(register_A); break;

		//XRA r
	case 0xa8: op_xor(register_B); break;
	case 0xa9: op_xor(register_C); break;
	case 0xaa: op_xor(register_D); break;
	case 0xab: op_xor(register_E); break;
	case 0xac: op_xor(register_H); break;
	case 0xad: op_xor(register_L); break;
	case 0xae: op_xor(memory[register_HL]); break;
	case 0xaf: op_xor(register_A); break;

		//ORA r
	case 0xb0: op_or(register_B); break;
	case 0xb1: op_or(register_C); break;
	case 0xb2: op_or(register_D); break;
	case 0xb3: op_or(register_E); break;
	case 0xb4: op_or(register_H); break;
	case 0xb5: op_or(register_L); break;
	case 0xb6: op_or(memory[register_HL]); break;
	case 0xb7: op_or(register_A); break;

		//CMP r
	case 0xb8: compareOp(register_B); break;
	case 0xb9: compareOp(register_C); break;
	case 0xba: compareOp(register_D); break;
	case 0xbb: compareOp(register_E); break;
	case 0xbc: compareOp(register_H); break;
	case 0xbd: compareOp(register_L); break;
	case 0xbe: compareOp(memory[register_HL]); break;
	case 0xbf: compareOp(register_A); break;

		//POP rp
	case 0xc1: register_BC = popWord(); break;
	case 0xd1: register_DE = popWord(); break;
	case 0xe1: register_HL = popWord(); break;
	case 0xf1: register_PSW = popWord(); break;

		//JMP d16
	case 0xc3: program_counter = getNextWord(); break;

		//JNZ d16
	case 0xc2:
		jump_target = getNextWord();
		if (Z == 0) program_counter = jump_target;
		break;

		//JNC d16
	case 0xd2:
		jump_target = getNextWord();
		if (C == 0) program_counter = jump_target;
		break;

		//JPO d16
	case 0xe2:
		jump_target = getNextWord();
		if (P == 0) program_counter = jump_target;
		break;

		//JP d16
	case 0xf2:
		jump_target = getNextWord();
		if (S == 0) program_counter = jump_target;
		break;

		//JZ d16
	case 0xca:
		jump_target = getNextWord();
		if (Z == 1) program_counter = jump_target;
		break;

		//JC d16
	case 0xda:
		jump_target = getNextWord();
		if (C == 1) program_counter = jump_target;
		break;

		//JPE d16
	case 0xea:
		jump_target = getNextWord();
		if (P == 1) program_counter = jump_target;
		break;

		//JM d16
	case 0xfa:
		jump_target = getNextWord();
		if (S == 1) program_counter = jump_target;
		break;

		//PUSH rp
	case 0xc5: pushWord(register_BC); break;
	case 0xd5: pushWord(register_DE); break;
	case 0xe5: pushWord(register_HL); break;
	case 0xf5: pushWord(register_PSW); break;

		//RET
	case 0xc9:
	case 0xd9: program_counter = popWord(); break;
	case 0xc0: if (Z == 0) program_counter = popWord(); break; //RNZ
	case 0xd0: if (C == 0) program_counter = popWord(); break; //RNC
	case 0xe0: if (P == 0) program_counter = popWord(); break; //RPO
	case 0xf0: if (S == 0) program_counter = popWord(); break; //RP
	case 0xc8: if (Z == 1) program_counter = popWord(); break; //RZ
	case 0xd8: if (C == 1) program_counter = popWord(); break; //RC
	case 0xe8: if (P == 1) program_counter = popWord(); break; //RPE
	case 0xf8: if (S == 1) program_counter = popWord(); break; //RM

	//CALL d16
	case 0xcd:
	case 0xdd:
	case 0xed:
	case 0xfd:
		jump_target = getNextWord();
		pushWord(program_counter);
		program_counter = jump_target;
		break;

		//CNZ d16
	case 0xc4:
		jump_target = getNextWord();
		if (Z == 0) {
			pushWord(program_counter);
			program_counter = jump_target;
		}
		break;

		//CNC d16
	case 0xd4:
		jump_target = getNextWord();
		if (C == 0) {
			pushWord(program_counter);
			program_counter = jump_target;
		}
		break;

		//CPO d16
	case 0xe4:
		jump_target = getNextWord();
		if (P == 0) {
			pushWord(program_counter);
			program_counter = jump_target;
		}
		break;

		//CP d16
	case 0xf4:
		jump_target = getNextWord();
		if (S == 0) {
			pushWord(program_counter);
			program_counter = jump_target;
		}
		break;

		//CZ d16
	case 0xcc:
		jump_target = getNextWord();
		if (Z == 1) {
			pushWord(program_counter);
			program_counter = jump_target;
		}
		break;

		//CC d16
	case 0xdc:
		jump_target = getNextWord();
		if (C == 1) {
			pushWord(program_counter);
			program_counter = jump_target;
		}
		break;

		//CPE d16
	case 0xec:
		jump_target = getNextWord();
		if (P == 1) {
			pushWord(program_counter);
			program_counter = jump_target;
		}
		break;

		//CM d16
	case 0xfc:
		jump_target = getNextWord();
		if (S == 1) {
			pushWord(program_counter);
			program_counter = jump_target;
		}
		break;

	case 0xd3: output_device(getNextByte(), register_A); break; //OUT d8
	case 0xdb: register_A = input_device(getNextByte()); break; //IN d8

	case 0xc6: op_add(getNextByte()); break; //ADI d8
	case 0xd6: op_sub(getNextByte()); break; //SUI d8
	case 0xe6: op_and(getNextByte()); break; //ANI d8
	case 0xf6: op_or(getNextByte()); break; //ORI d8
	case 0xce: register_A = addcOp(getNextByte()); break; //ACI d8
	case 0xde: register_A = subbOp(getNextByte()); break; //SBI d8
	case 0xee: op_xor(getNextByte()); break; //XRI d8
	case 0xfe: compareOp(getNextByte()); break; //CPI d8

	//XHTL
	case 0xe3: {
		uint8_t temp = register_L;
		register_L = memory[stack_pointer];
		memory[stack_pointer] = temp;

		temp = register_H;
		register_H = memory[stack_pointer + 1];
		memory[stack_pointer + 1] = temp;
		break;
	}

			   //PCHL
	case 0xe9: program_counter = register_HL; break;

		//XCHG
	case 0xeb: {
		uint16_t temp = register_DE;
		register_DE = register_HL;
		register_HL = temp;
		break;
	}

			   //SPHL
	case 0xf9: stack_pointer = register_HL; break;

	case 0xf3: interrupts_enabled = false; break; //DI
	case 0xfb: interrupts_enabled = true; break; //EI
	};
}