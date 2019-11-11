#include <stdio.h>
#include "cpu.h"

Intel8080::Intel8080(std::size_t mem_size) {
    memory = new uint8_t[mem_size];
}

Intel8080::~Intel8080() {
    delete[] memory;
}

void Intel8080::reset() {
    halted = false;
    interrupts_enabled = true;
    program_counter = 0x0000;
    stack_pointer = 0x0000;
}

void Intel8080::setInHandler(in_handler_t in) {
    in_callback = in;
}

void Intel8080::setOutHandler(out_handler_t out) {
    out_callback = out;
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
    const uint8_t instruction = memory[program_counter++];
    
    switch (instruction) {
        case 0x00: break;                                   // NOP
        case 0x01: register_BC = nextWord(); break;         // LXI B, d16
        case 0x02: memory[register_BC] = register_A; break; // STAX B
        case 0x03: ++register_BC; break;                    // INX B
        case 0x04: register_B = inr(register_B); break;     // INR B
        case 0x05: register_B = dcr(register_B); break;     // DCR B
        case 0x06: register_B = nextByte(); break;          // MVI B, d8
        case 0x07:                                          // RLC
            flag_C = (register_A & 0x80) == 0x80;
            register_A = (register_A << 1) | flag_C;    
            break;
        
        case 0x08: break;                                   // NOP
        case 0x09: dad(register_BC); break;                 // DAD B
        case 0x0a: register_A = memory[register_BC]; break; // LDAX B
        case 0x0b: --register_BC; break;                    // DCX B
        case 0x0c: register_C = inr(register_C); break;     // INR C
        case 0x0d: register_C = dcr(register_C); break;     // DCR C
        case 0x0e: register_C = nextByte(); break;          // MVI C, d8
        case 0x0f:                                          // RRC
            flag_C = (register_A & 0x01) == 0x01;
	        register_A = (register_A >> 1) | (flag_C << 7);
            break;

        case 0x10: break;                                   // NOP
        case 0x11: register_DE = nextWord(); break;         // LXI D, d16
        case 0x12: memory[register_DE] = register_A; break; // STAX D
        case 0x13: ++register_DE; break;                    // INX D
        case 0x14: register_D = inr(register_D); break;     // INR D
        case 0x15: register_D = dcr(register_D); break;     // DCR D
        case 0x16: register_D = nextByte(); break;          // MVI D, d8
        case 0x17: {                                        // RAL
            uint16_t result = register_A << 1;
            register_A = result | flag_C;
            flag_C = (result & 0x100) == 0x100;
        } break;
        
        case 0x18: break;                                   // NOP
        case 0x19: dad(register_DE); break;                 // DAD D
        case 0x1a: register_A = memory[register_DE]; break; // LDAX D
        case 0x1b: --register_DE; break;                    // DCX D
        case 0x1c: register_E = inr(register_E); break;     // INR E
        case 0x1d: register_E = dcr(register_E); break;     // DCR E
        case 0x1e: register_E = nextByte(); break;          // MVI E, d8
        case 0x1f: {                                        // RAR
            uint16_t result = register_A | (flag_C << 8);
            register_A = (result >> 1);
            flag_C = (result & 0x01) == 0x01;
        } break;

        case 0x20: break;                                   // NOP
        case 0x21: register_HL = nextWord(); break;         // LXI H, d16
        case 0x22: {                                        // SHLD
            uint16_t address = nextWord();
            memory[address] = register_HL;
            memory[address + 1] = register_HL >> 8;
        } break; 
        case 0x23: ++register_HL; break;                    // INX H
        case 0x24: register_H = inr(register_H); break;     // INR H
        case 0x25: register_H = dcr(register_H); break;     // DCR H
        case 0x26: register_H = nextByte(); break;          // MVI H, d8
        case 0x27: {                                        // DAA
            // TODO fix this opcode
            uint16_t result = register_A;
            if ((result & 0x0f) > 0x09 || flag_A) {
                result += 0x06;
                flag_A = (result ^ register_A) & 0x10;
            }
            if ((result & 0xf0) > 0x90 || flag_C) {
                result += 0x60;
            }
            flag_C = flag_C || result > 0x99;
            updateZSP(result);
            register_A = result;
        } break;
        
        case 0x28: break;                                   // NOP
        case 0x29: dad(register_HL); break;                 // DAD H
        case 0x2a: {                                        // LHLD
            uint16_t address = nextWord();
            register_HL = (memory[address + 1] << 8) | memory[address];   
        } break;
        case 0x2b: --register_HL; break;                    // DCX H
        case 0x2c: register_L = inr(register_L); break;     // INR L
        case 0x2d: register_L = dcr(register_L); break;     // DCR L
        case 0x2e: register_L = nextByte(); break;          // MVI L, d8
        case 0x2f: register_A = ~register_A; break;         // CMA

        case 0x30: break;                                   // NOP
        case 0x31: stack_pointer = nextWord(); break;       // LXI SP, d16
        case 0x32: memory[nextWord()] = register_A; break;  // STA d16
        case 0x33: ++stack_pointer; break;                  // INX SP
        case 0x34: memory[register_HL] = inr(memory[register_HL]); break; // INR H
        case 0x35: memory[register_HL] = dcr(memory[register_HL]); break; // DCR H
        case 0x36: memory[register_HL] = nextByte(); break; // MVI M, d8
        case 0x37: flag_C = true; break;                    // STC
        
        case 0x38: break;                                   // NOP
        case 0x39: dad(stack_pointer); break;               // DAD SP
        case 0x3a: register_A = memory[nextWord()]; break;  // LDA d16
        case 0x3b: --stack_pointer; break;                  // DCX SP
        case 0x3c: register_A = inr(register_A); break;     // INR A
        case 0x3d: register_A = dcr(register_A); break;     // DCR A
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

        case 0x80: add(register_B); break;                  // ADD B
        case 0x81: add(register_C); break;                  // ADD C
        case 0x82: add(register_D); break;                  // ADD D
        case 0x83: add(register_E); break;                  // ADD E
        case 0x84: add(register_H); break;                  // ADD H
        case 0x85: add(register_L); break;                  // ADD L
        case 0x86: add(memory[register_HL]); break;         // ADD M
        case 0x87: add(register_A); break;                  // ADD A

        case 0x88: adc(register_B); break;                  // ADC B
        case 0x89: adc(register_C); break;                  // ADC C
        case 0x8a: adc(register_D); break;                  // ADC D
        case 0x8b: adc(register_E); break;                  // ADC E
        case 0x8c: adc(register_H); break;                  // ADC H
        case 0x8d: adc(register_L); break;                  // ADC L
        case 0x8e: adc(memory[register_HL]); break;         // ADC M
        case 0x8f: adc(register_A); break;                  // ADC A

        case 0x90: sub(register_B); break;                  // SUB B
        case 0x91: sub(register_C); break;                  // SUB C
        case 0x92: sub(register_D); break;                  // SUB D
        case 0x93: sub(register_E); break;                  // SUB E
        case 0x94: sub(register_H); break;                  // SUB H
        case 0x95: sub(register_L); break;                  // SUB L
        case 0x96: sub(memory[register_HL]); break;         // SUB M
        case 0x97: sub(register_A); break;                  // SUB A

        case 0x98: sbb(register_B); break;                  // SBB B
        case 0x99: sbb(register_C); break;                  // SBB C
        case 0x9a: sbb(register_D); break;                  // SBB D
        case 0x9b: sbb(register_E); break;                  // SBB E
        case 0x9c: sbb(register_H); break;                  // SBB H
        case 0x9d: sbb(register_L); break;                  // SBB L
        case 0x9e: sbb(memory[register_HL]); break;         // SBB M
        case 0x9f: sbb(register_A); break;                  // SBB A

        case 0xa0: ana(register_B); break;                  // AND B
        case 0xa1: ana(register_C); break;                  // AND C
        case 0xa2: ana(register_D); break;                  // AND D
        case 0xa3: ana(register_E); break;                  // AND E
        case 0xa4: ana(register_H); break;                  // AND H        
        case 0xa5: ana(register_L); break;                  // AND L
        case 0xa6: ana(memory[register_HL]); break;         // AND M
        case 0xa7: ana(register_A); break;                  // AND A

        case 0xa8: xra(register_B); break;                  // XRA B
        case 0xa9: xra(register_C); break;                  // XRA C
        case 0xaa: xra(register_D); break;                  // XRA D
        case 0xab: xra(register_E); break;                  // XRA E
        case 0xac: xra(register_H); break;                  // XRA H
        case 0xad: xra(register_L); break;                  // XRA L
        case 0xae: xra(memory[register_HL]); break;         // XRA M
        case 0xaf: xra(register_A); break;                  // XRA A

        case 0xb0: ora(register_B); break;                  // ORA B
        case 0xb1: ora(register_C); break;                  // ORA C
        case 0xb2: ora(register_D); break;                  // ORA D
        case 0xb3: ora(register_E); break;                  // ORA E
        case 0xb4: ora(register_H); break;                  // ORA H        
        case 0xb5: ora(register_L); break;                  // ORA L
        case 0xb6: ora(memory[register_HL]); break;         // ORA M
        case 0xb7: ora(register_A); break;                  // ORA A

        case 0xb8: cmp(register_B); break;                  // CMP B
        case 0xb9: cmp(register_C); break;                  // CMP C
        case 0xba: cmp(register_D); break;                  // CMP D
        case 0xbb: cmp(register_E); break;                  // CMP E
        case 0xbc: cmp(register_H); break;                  // CMP H
        case 0xbd: cmp(register_L); break;                  // CMP L
        case 0xbe: cmp(memory[register_HL]); break;         // CMP M
        case 0xbf: cmp(register_A); break;                  // CMP A

        case 0xc0: ret(!flag_Z); break;                     // RNZ
        case 0xc1: register_BC = pop(); break;              // POP B
        case 0xc2: jmp(!flag_Z); break;                     // JNZ d15
        case 0xc3: jmp(true); break;                        // JMP d16
        case 0xc4: call(!flag_Z); break;                    // CNZ d16
        case 0xc5: push(register_BC); break;                // PUSH B
        case 0xc6: add(nextByte()); break;                  // ADI d8

        case 0xc8: ret(flag_Z); break;                      // RZ
        case 0xc9: ret(true); break;                        // RET
        case 0xca: jmp(flag_Z); break;                      // JZ d16
        case 0xcc: call(flag_Z); break;                     // CZ d16
        case 0xcd: call(true); break;                       // CALL d16
        case 0xce: adc(nextByte()); break;                  // ACI d8

        case 0xd0: ret(!flag_C); break;                     // RNC
        case 0xd1: register_DE = pop(); break;              // POP D
        case 0xd2: jmp(!flag_C); break;                     // JNC d16
        case 0xd3: out_callback(nextByte(), register_A); break; // OUT d8
        case 0xd4: call(!flag_C); break;                    // CNC d16
        case 0xd5: push(register_DE); break;                // PUSH D
        case 0xd6: sub(nextByte()); break;                  // SUI d6

        case 0xd8: ret(flag_C); break;                      // RC d16
        case 0xda: jmp(flag_C); break;                      // JC d16
        case 0xdb: register_A = in_callback(nextByte()); break; // IN d8
        case 0xdc: call(flag_C); break;                     // CC d16
        case 0xde: sbb(nextByte()); break;                  // SBI d16

        case 0xe0: ret(!flag_P); break;                     // RNZ
        case 0xe1: register_HL = pop(); break;              // POP H
        case 0xe2: jmp(!flag_P); break;                     // JPE d16
        case 0xe3: {                                        // XTHL
            // TODO not portable
            uint16_t temp = *(uint16_t *) &memory[stack_pointer];
            *(uint16_t *) &memory[stack_pointer] = register_HL;
            register_HL = temp;
        } break;
        case 0xe4: call(!flag_P); break;                    // JPO d16
        case 0xe5: push(register_HL); break;                // PUSH H
        case 0xe6: ana(nextByte()); break;                  // ANI d8

        case 0xe8: ret(flag_P); break;                      // RPE
        case 0xe9: program_counter = register_HL; break;    // PCHL
        case 0xea: jmp(flag_P); break;                      // JPE d16
        case 0xeb: {                                        // XCHG
            uint16_t temp = register_HL;
            register_HL = register_DE;
            register_DE = temp;
        } break;
        case 0xec: call(flag_P); break;                     // CPE d16
        case 0xee: xra(nextByte()); break;                  // XRI d8

        case 0xf0: ret(!flag_S); break;                     // RP
        case 0xf1:                                          // POP PSW
            register_PSW = pop(); 
            loadFlags(); 
            break;
        case 0xf2: jmp(!flag_S); break;                     // JPE d16
        case 0xf3: interrupts_enabled = false; break;
        case 0xf4: call(!flag_S); break;                    // CP d16
        case 0xf5:                                          // PUSH PSW
            storeFlags(); 
            push(register_PSW); 
            break;
        case 0xf6: ora(nextByte()); break;                  // ORI d8

        case 0xf8: ret(flag_S); break;                      // RM
        case 0xf9: stack_pointer = register_HL; break;      // SPHL
        case 0xfa: jmp(flag_S); break;                      // JM d16
        case 0xfb: interrupts_enabled = true; break;
        case 0xfc: call(flag_S); break;                     // CM d16
        case 0xfe: cmp(nextByte()); break;                  // CPI d8

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
    // TODO indexing not overflow safe
    return (memory[program_counter - 1] << 8) | memory[program_counter - 2]; 
}

void Intel8080::push(uint16_t word) {
    memory[stack_pointer - 2] = word;
    memory[stack_pointer - 1] = word >> 8;
    stack_pointer -= 2;
}

uint16_t Intel8080::pop() {
    stack_pointer += 2;
    // TODO indexing not overflow safe
    return (memory[stack_pointer - 1] << 8) | memory[stack_pointer - 2]; 
}

void Intel8080::updateZSP(const uint8_t result) {
    flag_S = (int8_t) result < 0;
	flag_Z = result == 0;
	flag_P = (0x9669 >> ((result ^ (result >> 4)) & 0x0f)) & 1;
}

void Intel8080::storeFlags() {
    flags = 0x02;
    flags |= flag_S << 7;
    flags |= flag_Z << 6;
    flags |= flag_A << 4;
    flags |= flag_P << 2;
    flags |= flag_C;
}

void Intel8080::loadFlags() {
    flag_S = flags & 0x80;
    flag_Z = flags & 0x40;
    flag_A = flags & 0x10;
    flag_P = flags & 0x04;
    flag_C = flags & 0x01;
}

uint8_t Intel8080::inr(uint8_t value) {
    value += 1;
    updateZSP(value);
	flag_A = (value & 0xf) == 0;
    return value;
}

uint8_t Intel8080::dcr(uint8_t value) {
    value -= 1;
    updateZSP(value);
	flag_A = (value & 0xf) != 0xf;
    return value;
}

void Intel8080::add(const uint8_t value) {
    uint16_t result = register_A + value;
    updateZSP(result);
	flag_A = (result ^ register_A ^ value) & 0x10;
    flag_C = result > 0xff;
    register_A = result;
}

void Intel8080::adc(const uint8_t value) {
    uint16_t result = register_A + value + flag_C;
    updateZSP(result);
	flag_A = (result ^ register_A ^ value) & 0x10;
    flag_C = result > 0xff;
    register_A = result;
}

void Intel8080::sub(const uint8_t value) {
    uint16_t result = register_A - value;
    updateZSP(result);
	flag_A = ~(result ^ register_A ^ value) & 0x10;
    flag_C = result > 0xff;
    register_A = result;
}

void Intel8080::sbb(const uint8_t value) {
    uint16_t result = register_A - value - flag_C;
    updateZSP(result);
	flag_A = ~(result ^ register_A ^ value) & 0x10;
    flag_C = result > 0xff;
    register_A = result;
}

void Intel8080::ana(const uint8_t value) {
    flag_A = (register_A | value) & 0x08;
    flag_C = 0;
    register_A &= value;
    updateZSP(register_A);
}

void Intel8080::xra(const uint8_t value) {
    flag_A = 0;
    flag_C = 0;
    register_A ^= value;
    updateZSP(register_A);
}

void Intel8080::ora(const uint8_t value) {
    flag_A = 0;
    flag_C = 0;
    register_A |= value;
    updateZSP(register_A);
}

void Intel8080::cmp(const uint8_t value) {
    uint16_t result = register_A - value;
    updateZSP(result);
	flag_A = ~(result ^ register_A ^ value) & 0x10;
    flag_C = result > 0xff;
}

void Intel8080::dad(const uint16_t value) {
    register_HL += value;
    flag_C = register_HL < value;
}

void Intel8080::jmp(const bool condition) {
    uint16_t jump_target = nextWord();
    if (condition) {
        program_counter = jump_target;
    }
}

void Intel8080::call(const bool condition) {
    uint16_t jump_target = nextWord();
    if (condition) {
        push(program_counter);
        program_counter = jump_target;
    }
}

void Intel8080::ret(const bool condition) {
    if (condition) {
        program_counter = pop();
    }
}