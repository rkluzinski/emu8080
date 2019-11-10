#include <stdio.h>
#include "cpu.h"

Intel8080::Intel8080(std::size_t mem_size) {
    memory = new uint8_t[mem_size];
    reset();
}

Intel8080::~Intel8080() {
    delete[] memory;
}

void Intel8080::reset() {
    halted = false;
    //program_counter = 0;
    program_counter = 0x100; // TODO change later
}

void Intel8080::setInHandler(in_handler_t in) {
    in_handler = in;
}

void Intel8080::setOutHandler(out_handler_t out) {
    out_handler = out;
}

void Intel8080::dump() {
    // dump program counter, stack pointer, registers
    printf("PC  = 0x%.4x\n", program_counter);
    printf("SP  = 0x%.4x\n", stack_pointer);
    printf("BC  = 0x%.4x\n", register_BC);
    printf("DE  = 0x%.4x\n", register_DE);
    printf("HL  = 0x%.4x\n", register_HL);
    printf("PSW = 0x%.4x\n", register_PSW);
    
    // dump current instruction
    printf("Current instruction: 0x%.2x\n", memory[program_counter]);
}

std::size_t Intel8080::executeInstruction() {
    uint8_t instruction = memory[program_counter++];
    
    switch (instruction) {
        case 0x00: break;                           // NOP
        case 0x01: register_BC = nextWord(); break; // LXI B, d16
        case 0x02: memory[register_BC] = register_A; break; // STAX B
        case 0x03: ++register_BC; break;            // INX B
        case 0x04: INC(register_B); break;    // INR B
        case 0x05: DCR(register_B); break;    // DCR B
        case 0x06: register_B = nextByte(); break;  // MVI B, d8
        case 0x07: // RLC
            flag_C = (register_A & 0x80) == 0x80;
            register_A = (register_A << 1) | flag_C;    
            break;
        
        case 0x08: break;                           // NOP
        case 0x09: _dadd(register_BC); break;       // DAD B
        case 0x0a: register_A = memory[register_BC]; break; // LDAX B
        case 0x0b: --register_BC; break;            // DCX B
        case 0x0c: INC(register_C); break;    // INR C
        case 0x0d: DCR(register_C); break;    // DCR C
        case 0x0e: register_C = nextByte(); break;  // MVI C, d8
        case 0x0f: // RRC
            flag_C = (register_A & 0x01) == 0x01;
	        register_A = (register_A >> 1) | (flag_C << 7);
            break;

        case 0x10: break;                           // NOP
        case 0x11: register_DE = nextWord(); break; // LXI D, d16
        case 0x12: memory[register_DE] = register_A; break; // STAX D
        case 0x13: ++register_DE; break;            // INX D
        case 0x14: INC(register_D); break;    // INR D
        case 0x15: DCR(register_D); break;    // DCR D
        case 0x16: register_D = nextByte(); break;  // MVI D, d8
        case 0x17: { // RAL
            uint16_t result = register_A << 1;
            register_A = result | flag_C;
            flag_C = (result & 0x100) == 0x100;
        } break;
        
        case 0x18: break;                                   // NOP
        case 0x19: _dadd(register_DE); break;               // DAD D
        case 0x1a: register_A = memory[register_DE]; break; // LDAX D
        case 0x1b: --register_DE; break;                    // DCX D
        case 0x1c: INC(register_E); break;            // INR E
        case 0x1d: DCR(register_E); break;            // DCR E
        case 0x1e: register_E = nextByte(); break;          // MVI E, d8
        case 0x1f: { // RAR
            uint16_t result = register_A | (flag_C << 8);
            register_A = (result >> 1);
            flag_C = (result & 0x01) == 0x01;
        } break;

        case 0x20: break;                           // NOP
        case 0x21: register_HL = nextWord(); break; // LXI H, d16
        // TODO fix this
        case 0x22: *((uint16_t*) &memory[nextWord()]) = register_HL; break; // SHLD
        case 0x23: ++register_HL; break;            // INX H
        case 0x24: INC(register_H); break;    // INR H
        case 0x25: DCR(register_H); break;    // DCR H
        case 0x26: register_H = nextByte(); break;  // MVI H, d8
        case 0x27: // DAA
            if ((register_A & 0x0f) > 0x09 || flag_A) {
                register_A += 0x06;
                flag_A = (register_A & 0xf) < 0x06;
            }
            if ((register_A & 0xf0) > 0x90 || flag_C) {
                register_A += 0x60;
                flag_C = register_A < 0x60;
            }

            flag_S = (int8_t) register_A < 0;
            flag_Z = register_A == 0;
            flag_P = (0x9669 >> ((register_A ^ (register_A >> 4)) & 0x0f)) & 1;
            break;
        
        case 0x28: break;                           // NOP
        case 0x29: _dadd(register_HL); break;       // DAD H
        // TODO fix this
        case 0x2a: register_HL = *((uint16_t*) &memory[nextWord()]); break; // LHLD
        case 0x2b: --register_HL; break;            // DCX H
        case 0x2c: INC(register_L); break;    // INR L
        case 0x2d: DCR(register_L); break;    // DCR L
        case 0x2e: register_L = nextByte(); break;  // MVI L, d8
        case 0x2f: register_A = ~register_A; break; // CMA

        case 0x30: break;                                   // NOP
        case 0x31: stack_pointer = nextWord(); break;       // LXI SP, d16
        case 0x32: memory[nextWord()] = register_A; break;  // STA d16
        case 0x33: ++stack_pointer; break;                  // INX SP
        case 0x34: INC(memory[register_HL]); break;   // INR H
        case 0x35: DCR(memory[register_HL]); break;   // DCR H
        case 0x36: memory[register_HL] = nextByte(); break; // MVI M, d8
        case 0x37: flag_C = true; break;                    // STC
        
        case 0x38: break;                                   // NOP
        case 0x39: _dadd(stack_pointer); break;             // DAD SP
        case 0x3a: register_A = memory[nextWord()]; break;  // LDA d16
        case 0x3b: --stack_pointer; break;                  // DCX SP
        case 0x3c: INC(register_A); break;            // INR A
        case 0x3d: DCR(register_A); break;            // DCR A
        case 0x3e: register_A = nextByte(); break;          // MVI C, d8
        case 0x3f: flag_C = !flag_C; break;                 // CMC

        case 0x40: register_B = register_B; break;          // MOV B, B
        case 0x41: register_B = register_C; break;          // MOV B, C
        case 0x42: register_B = register_D; break;          // MOV B, D
        case 0x43: register_B = register_E; break;          // MOV B, E
        case 0x44: register_B = register_H; break;          // MOV B, H
        case 0x45: register_B = register_L; break;          // MOV B, L
        case 0x46: register_B = memory[register_HL]; break; // MOV B, M
        case 0x47: register_B = register_A; break;          // MOV B, A

        case 0x48: register_C = register_B; break;          // MOV C, B
        case 0x49: register_C = register_C; break;          // MOV C, C
        case 0x4a: register_C = register_D; break;          // MOV C, D
        case 0x4b: register_C = register_E; break;          // MOV C, E
        case 0x4c: register_C = register_H; break;          // MOV C, H
        case 0x4d: register_C = register_L; break;          // MOV C, L
        case 0x4e: register_C = memory[register_HL]; break; // MOV C, M
        case 0x4f: register_C = register_A; break;          // MOV C, A

        case 0x50: register_D = register_B; break;          // MOV D, B
        case 0x51: register_D = register_C; break;          // MOV D, C
        case 0x52: register_D = register_D; break;          // MOV D, D
        case 0x53: register_D = register_E; break;          // MOV D, E
        case 0x54: register_D = register_H; break;          // MOV D, H
        case 0x55: register_D = register_L; break;          // MOV D, L
        case 0x56: register_D = memory[register_HL]; break; // MOV D, M
        case 0x57: register_D = register_A; break;          // MOV D, A

        case 0x58: register_E = register_B; break;          // MOV E, B
        case 0x59: register_E = register_C; break;          // MOV E, C
        case 0x5a: register_E = register_D; break;          // MOV E, D
        case 0x5b: register_E = register_E; break;          // MOV E, E
        case 0x5c: register_E = register_H; break;          // MOV E, H
        case 0x5d: register_E = register_L; break;          // MOV E, L
        case 0x5e: register_E = memory[register_HL]; break; // MOV E, M
        case 0x5f: register_E = register_A; break;          // MOV E, A

        case 0x60: register_H = register_B; break;          // MOV H, B
        case 0x61: register_H = register_C; break;          // MOV H, C
        case 0x62: register_H = register_D; break;          // MOV H, D
        case 0x63: register_H = register_E; break;          // MOV H, E
        case 0x64: register_H = register_H; break;          // MOV H, H
        case 0x65: register_H = register_L; break;          // MOV H, L
        case 0x66: register_H = memory[register_HL]; break; // MOV H, M
        case 0x67: register_H = register_A; break;          // MOV H, A

        case 0x68: register_L = register_B; break;          // MOV L, B
        case 0x69: register_L = register_C; break;          // MOV L, C
        case 0x6a: register_L = register_D; break;          // MOV L, D
        case 0x6b: register_L = register_E; break;          // MOV L, E
        case 0x6c: register_L = register_H; break;          // MOV L, H
        case 0x6d: register_L = register_L; break;          // MOV L, L
        case 0x6e: register_L = memory[register_HL]; break; // MOV L, M
        case 0x6f: register_L = register_A; break;          // MOV L, A

        case 0x70: memory[register_HL] = register_B; break; // MOV M, B
        case 0x71: memory[register_HL] = register_C; break; // MOV M, C
        case 0x72: memory[register_HL] = register_D; break; // MOV M, D
        case 0x73: memory[register_HL] = register_E; break; // MOV M, E
        case 0x74: memory[register_HL] = register_H; break; // MOV M, H
        case 0x75: memory[register_HL] = register_L; break; // MOV M, L
        case 0x76: halted = true; break;                    // HLT
        case 0x77: memory[register_HL] = register_A; break; // MOV M, A

        case 0x78: register_A = register_B; break;          // MOV A, B
        case 0x79: register_A = register_C; break;          // MOV A, C
        case 0x7a: register_A = register_D; break;          // MOV A, D
        case 0x7b: register_A = register_E; break;          // MOV A, E
        case 0x7c: register_A = register_H; break;          // MOV A, H
        case 0x7d: register_A = register_L; break;          // MOV A, L
        case 0x7e: register_A = memory[register_HL]; break; // MOV A, M
        case 0x7f: register_A = register_A; break;          // MOV A, A

        case 0x80: _add(register_B); break; // ADD B
        case 0x81: _add(register_C); break; // ADD C
        case 0x82: _add(register_D); break; // ADD D
        case 0x83: _add(register_E); break; // ADD E
        case 0x84: _add(register_H); break; // ADD H
        case 0x85: _add(register_L); break; // ADD L
        case 0x86: _add(memory[register_HL]); break; // ADD M
        case 0x87: _add(register_A); break; // ADD A

        case 0x88: _add(register_B, flag_C); break; // ADC B
        case 0x89: _add(register_C, flag_C); break; // ADC C
        case 0x8a: _add(register_D, flag_C); break; // ADC D
        case 0x8b: _add(register_E, flag_C); break; // ADC E
        case 0x8c: _add(register_H, flag_C); break; // ADC H
        case 0x8d: _add(register_L, flag_C); break; // ADC L
        case 0x8e: _add(memory[register_HL], flag_C); break; // ADC M
        case 0x8f: _add(register_A, flag_C); break; // ADC A

        case 0x90: _sub(register_B); break; // SUB B
        case 0x91: _sub(register_C); break; // SUB C
        case 0x92: _sub(register_D); break; // SUB D
        case 0x93: _sub(register_E); break; // SUB E
        case 0x94: _sub(register_H); break; // SUB H
        case 0x95: _sub(register_L); break; // SUB L
        case 0x96: _sub(memory[register_HL]); break; // SUB M
        case 0x97: _sub(register_A); break; // SUB A

        case 0x98: _sub(register_B, flag_C); break; // SBB B
        case 0x99: _sub(register_C, flag_C); break; // SBB C
        case 0x9a: _sub(register_D, flag_C); break; // SBB D
        case 0x9b: _sub(register_E, flag_C); break; // SBB E
        case 0x9c: _sub(register_H, flag_C); break; // SBB H
        case 0x9d: _sub(register_L, flag_C); break; // SBB L
        case 0x9e: _sub(memory[register_HL], flag_C); break; // SBB M
        case 0x9f: _sub(register_A, flag_C); break; // SBB A

        case 0xa0: _and(register_B); break; // AND B
        case 0xa1: _and(register_C); break; // AND C
        case 0xa2: _and(register_D); break; // AND D
        case 0xa3: _and(register_E); break; // AND E
        case 0xa4: _and(register_H); break; // AND H        
        case 0xa5: _and(register_L); break; // AND L
        case 0xa6: _and(memory[register_HL]); break;    // AND M
        case 0xa7: _and(register_A); break; // AND A

        case 0xa8: _xor(register_B); break; // XOR B
        case 0xa9: _xor(register_C); break; // XOR C
        case 0xaa: _xor(register_D); break; // XOR D
        case 0xab: _xor(register_E); break; // XOR E
        case 0xac: _xor(register_H); break; // XOR H
        case 0xad: _xor(register_L); break; // XOR L
        case 0xae: _xor(memory[register_HL]); break;    // XOR M
        case 0xaf: _xor(register_A); break; // XOR A

        case 0xb0: _or(register_B); break; // OR B
        case 0xb1: _or(register_C); break; // OR C
        case 0xb2: _or(register_D); break; // OR D
        case 0xb3: _or(register_E); break; // OR E
        case 0xb4: _or(register_H); break; // OR H        
        case 0xb5: _or(register_L); break; // OR L
        case 0xb6: _or(memory[register_HL]); break;    // OR M
        case 0xb7: _or(register_A); break; // OR A

        case 0xb8: _compare(register_B); break; // CMP B
        case 0xb9: _compare(register_C); break; // CMP C
        case 0xba: _compare(register_D); break; // CMP D
        case 0xbb: _compare(register_E); break; // CMP E
        case 0xbc: _compare(register_H); break; // CMP H
        case 0xbd: _compare(register_L); break; // CMP L
        case 0xbe: _compare(memory[register_HL]); break;    // CMP M
        case 0xbf: _compare(register_A); break; // CMP A

        case 0xc0: _return(!flag_Z); break; // RNZ
        case 0xc1: register_BC = popWord(); break;   // POP B
        case 0xc2: _jump(!flag_Z); break;   // JNZ d15
        case 0xc3: _jump(true); break;          // JMP d16
        case 0xc4: _call(!flag_Z); break;   // CNZ d16
        case 0xc5: pushWord(register_BC); break; // PUSH B
        case 0xc6: _add(nextByte()); break;     // ADI d8

        case 0xc8: _return(flag_Z); break;  // RZ
        case 0xc9: _return(true); break;        // RET
        case 0xca: _jump(flag_Z); break;    // JZ d16
        case 0xcc: _call(flag_Z); break;    // CZ d16
        case 0xcd: _call(true); break;          // CALL d16
        case 0xce: _add(nextByte(), flag_C); break;    // ACI d8

        case 0xd0: _return(!flag_C); break;                // RNC
        case 0xd1: register_DE = popWord(); break;              // POP D
        case 0xd2: _jump(!flag_C); break;                  // JNC d16
        case 0xd3: out_handler(nextByte(), register_A); break;  // OUT d8
        case 0xd4: _call(!flag_C); break;                  // CNC d16
        case 0xd5: pushWord(register_DE); break;                // PUSH D
        case 0xd6: _sub(nextByte()); break;                     // SUI d6

        case 0xd8: _return(flag_C); break; // RC d16
        case 0xda: _jump(flag_C); break;   // JC d16
        case 0xdc: _call(flag_C); break;   // CC d16
        case 0xde: _sub(nextByte(), flag_C); break;    // SBI d16

        case 0xe0: _return(!flag_P); break;     // RNZ
        case 0xe1: register_HL = popWord(); break;  // POP H
        case 0xe2: _jump(!flag_P); break;     // JPE d16
        case 0xe3: { // XTHL
            uint16_t temp = *(uint16_t *) &memory[stack_pointer];
            *(uint16_t *) &memory[stack_pointer] = register_HL;
            register_HL = temp;
        } break;
        case 0xe4: _call(!flag_P); break;     // JPO d16
        case 0xe5: pushWord(register_HL); break;    // PUSH H
        case 0xe6: _and(nextByte()); break;         // ANI d8

        case 0xe8: _return(flag_P); break;    // RPE
        case 0xe9: program_counter = register_HL; break;    // PCHL
        case 0xea: _jump(flag_P); break;      // JPE d16
        case 0xeb: { // XCHG
            uint16_t temp = register_HL;
            register_HL = register_DE;
            register_DE = temp;
        } break;
        case 0xec: _call(flag_P); break;      // CPE d16
        case 0xee: _xor(nextByte()); break;         // XRI d8

        case 0xf0: _return(!flag_S); break;     // RP
        case 0xf1: register_PSW = popWord(); break; // POP PSW
        case 0xf2: _jump(!flag_S); break;       // JPE d16
        case 0xf3: interrupts_enabled = false; break;
        case 0xf4: _call(!flag_S); break;       // CP d16
        case 0xf5: pushWord(register_PSW); break;   // PUSH PSW
        case 0xf6: _or(nextByte()); break;          // ORI d8

        case 0xf8: _return(flag_S); break;  // RM
        case 0xf9: stack_pointer = register_HL; break; // SPHL
        case 0xfa: _jump(flag_S); break;    // JM d16
        case 0xfb: interrupts_enabled = true; break;
        case 0xfc: _call(flag_S); break;    // CM d16
        case 0xfe: _compare(nextByte()); break; // CPI d8

        default:
            program_counter--;  // restore program counter
            throw Intel8080Exception("Unimplemented Instruction");
    }

    // TODO return real cycle count
    return 1;
}

std::size_t Intel8080::execute() {
    std::size_t cycle_count = 0;
    
    while (!halted) {
        cycle_count += executeInstruction();
    }

    return cycle_count;
}

uint8_t Intel8080::nextByte() {
    return memory[program_counter++];
}

uint16_t Intel8080::nextWord() {
    program_counter += 2;
    return (memory[program_counter - 1] << 8) | memory[program_counter - 2]; 
}

void Intel8080::pushWord(uint16_t word) {
    stack_pointer -= 2;
    memory[stack_pointer] = word;
    memory[stack_pointer + 1] = word >> 8;
}

uint16_t Intel8080::popWord() {
    stack_pointer += 2;
    return (memory[stack_pointer - 1] << 8) | memory[stack_pointer - 2]; 
}

void Intel8080::INC(uint8_t &dst) {
    dst += 1;
    flag_S = (int8_t) dst < 0;
	flag_Z = dst == 0;
	flag_A = (dst & 0xf) == 0;
	flag_P = (0x9669 >> ((dst ^ (dst >> 4)) & 0x0f)) & 1;
    flag_C = flag_Z;
}

void Intel8080::DCR(uint8_t &dst) {
    dst -= 1;
    flag_S = (int8_t) dst < 0;
	flag_Z = dst == 0;
	flag_A = (dst & 0xf) == 0;
	flag_P = (0x9669 >> ((dst ^ (dst >> 4)) & 0x0f)) & 1;
    flag_C = flag_Z;
}

void Intel8080::_add(const uint8_t src, const uint8_t carry) {
    register_A = register_A + src + carry;
    flag_S = (int8_t) register_A < 0;
	flag_Z = register_A == 0;
	flag_A = (register_A & 0xf) < (src & 0xf);
	flag_P = (0x9669 >> ((register_A ^ (register_A >> 4)) & 0x0f)) & 1;
    flag_C = register_A < src;
}

void Intel8080::_sub(const uint8_t src, const uint8_t carry) {
    flag_C = register_A < src + carry;
    flag_A = (register_A & 0xf) < ((src + carry) & 0xf);
    register_A = register_A - src - carry;
    flag_S = (int8_t) register_A < 0;
	flag_Z = register_A == 0;
	flag_P = (0x9669 >> ((register_A ^ (register_A >> 4)) & 0x0f)) & 1;
}

void Intel8080::_and(const uint8_t src) {
    register_A = register_A & src;
    flag_S = (int8_t) register_A < 0;
	flag_Z = register_A == 0;
	flag_P = (0x9669 >> ((register_A ^ (register_A >> 4)) & 0x0f)) & 1;
    flag_C = 0;
}

void Intel8080::_xor(const uint8_t src) {
    register_A = register_A ^ src;
    flag_S = (int8_t) register_A < 0;
	flag_Z = register_A == 0;
	flag_P = (0x9669 >> ((register_A ^ (register_A >> 4)) & 0x0f)) & 1;
    flag_C = 0;
}

void Intel8080::_or(const uint8_t src) {
    register_A = register_A | src;
    flag_S = (int8_t) register_A < 0;
	flag_Z = register_A == 0;
	flag_P = (0x9669 >> ((register_A ^ (register_A >> 4)) & 0x0f)) & 1;
    flag_C = 0;
}

void Intel8080::_compare(const uint8_t src) {
    uint8_t result = register_A - src;
    flag_S = (int8_t) result < 0;
	flag_Z = result == 0;
	flag_A = (register_A & 0xf) < (src & 0xf);
	flag_P = (0x9669 >> ((result ^ (result >> 4)) & 0x0f)) & 1;
    flag_C = register_A < src;
}

void Intel8080::_dadd(const uint16_t src) {
    register_HL += src;
    flag_C = register_HL < src;
}

void Intel8080::_jump(const bool condition) {
    uint16_t jump_target = nextWord();
    if (condition) {
        program_counter = jump_target;
    }
}

void Intel8080::_call(const bool condition) {
    uint16_t jump_target = nextWord();
    if (condition) {
        pushWord(program_counter);
        program_counter = jump_target;
    }
}

void Intel8080::_return(const bool condition) {
    if (condition) {
        program_counter = popWord();
    }
}