#include "cpu.h"

void Intel8080::reset() {
    halted = false;
    interrupts_enabled = true;
    program_counter = 0x0000;
    stack_pointer = 0x0000;
}

std::size_t Intel8080::execute() {
    std::size_t cycles = 0;
    while (!halted)
        cycles += executeInstruction();
    return cycles;
}

std::size_t Intel8080::execute(std::size_t target_cycles) {
    std::size_t cycles = 0;
    while (!halted && cycles < target_cycles)
        cycles += executeInstruction();
    return cycles;
}

std::size_t Intel8080::executeInstruction() {
    const uint8_t instruction = memory[program_counter++];

    switch (instruction) {
    case 0x00: // NOP
        break;
    case 0x01: // LXI B, d16
        register_BC = nextWord();
        break;
    case 0x02: // STAX B
        memory[register_BC] = register_A;
        break;
    case 0x03: // INX B
        ++register_BC;
        break;
    case 0x04: // INR B
        register_B = inr(register_B);
        break;
    case 0x05: // DCR B
        register_B = dcr(register_B);
        break;
    case 0x06: // MVI B, d8
        register_B = nextByte();
        break;
    case 0x07: // RLC
        flag_C = (register_A & 0x80) == 0x80;
        register_A = (register_A << 1) | flag_C;
        break;

    case 0x08: // NOP
        break;
    case 0x09: // DAD B
        dad(register_BC);
        break;
    case 0x0a: // LDAX B
        register_A = memory[register_BC];
        break;
    case 0x0b: // DCX B
        --register_BC;
        break;
    case 0x0c: // INR C
        register_C = inr(register_C);
        break;
    case 0x0d: // DCR C
        register_C = dcr(register_C);
        break;
    case 0x0e: // MVI C, d8
        register_C = nextByte();
        break;
    case 0x0f: // RRC
        flag_C = (register_A & 0x01) == 0x01;
        register_A = (register_A >> 1) | (flag_C << 7);
        break;

    case 0x10: // NOP
        break;
    case 0x11: // LXI D, d16
        register_DE = nextWord();
        break;
    case 0x12: // STAX D
        memory[register_DE] = register_A;
        break;
    case 0x13: // INX D
        ++register_DE;
        break;
    case 0x14: // INR D
        register_D = inr(register_D);
        break;
    case 0x15: // DCR D
        register_D = dcr(register_D);
        break;
    case 0x16: // MVI D, d8
        register_D = nextByte();
        break;
    case 0x17: // RAL
    {
        uint16_t result = register_A << 1;
        register_A = result | flag_C;
        flag_C = (result & 0x100) == 0x100;
    } break;

    case 0x18: // NOP
        break;
    case 0x19: // DAD D
        dad(register_DE);
        break;
    case 0x1a: // LDAX D
        register_A = memory[register_DE];
        break;
    case 0x1b: // DCX D
        --register_DE;
        break;
    case 0x1c: // INR E
        register_E = inr(register_E);
        break;
    case 0x1d: // DCR E
        register_E = dcr(register_E);
        break;
    case 0x1e: // MVI E, d8
        register_E = nextByte();
        break;
    case 0x1f: // RAR
    {
        uint16_t result = register_A | (flag_C << 8);
        register_A = (result >> 1);
        flag_C = (result & 0x01) == 0x01;
    } break;

    case 0x20: // NOP
        break;
    case 0x21: // LXI H, d16
        register_HL = nextWord();
        break;
    case 0x22: // SHLD
    {
        uint16_t address = nextWord();
        memory[address] = register_HL;
        memory[++address] = register_HL >> 8;
    } break;
    case 0x23: // INX H
        ++register_HL;
        break;
    case 0x24: // INR H
        register_H = inr(register_H);
        break;
    case 0x25: // DCR H
        register_H = dcr(register_H);
        break;
    case 0x26: // MVI H, d8
        register_H = nextByte();
        break;
    case 0x27: // DAA
        /**
         * Reordering DAA allows the program to operate
         * directly on the register_A rather than an
         * intermediate variable.
         */
        if (flag_C || register_A > 0x99) {
            flag_C = true;
            register_A += 0x60;
        }
        if (flag_A || (register_A & 0xf) > 0x9) {
            flag_A = (register_A & 0xf) > 0x9;
            register_A += 0x06;
        }
        updateZSP(register_A);
        break;

    case 0x28: // NOP
        break;
    case 0x29: // DAD H
        dad(register_HL);
        break;
    case 0x2a: // LHLD
    {
        uint16_t address = nextWord();
        register_L = memory[address];
        register_H = memory[++address];
    } break;
    case 0x2b: // DCX H
        --register_HL;
        break;
    case 0x2c: // INR L
        register_L = inr(register_L);
        break;
    case 0x2d: // DCR L
        register_L = dcr(register_L);
        break;
    case 0x2e: // MVI L, d8
        register_L = nextByte();
        break;
    case 0x2f: // CMA
        register_A = ~register_A;
        break;

    case 0x30: // NOP
        break;
    case 0x31: // LXI SP, d16
        stack_pointer = nextWord();
        break;
    case 0x32: // STA d16
        memory[nextWord()] = register_A;
        break;
    case 0x33: // INX SP
        ++stack_pointer;
        break;
    case 0x34: // INR H
        memory[register_HL] = inr(memory[register_HL]);
        break;
    case 0x35: // DCR H
        memory[register_HL] = dcr(memory[register_HL]);
        break;
    case 0x36: // MVI M, d8
        memory[register_HL] = nextByte();
        break;
    case 0x37: // STC
        flag_C = true;
        break;

    case 0x38: // NOP
        break;
    case 0x39: // DAD SP
        dad(stack_pointer);
        break;
    case 0x3a: // LDA d16
        register_A = memory[nextWord()];
        break;
    case 0x3b: // DCX SP
        --stack_pointer;
        break;
    case 0x3c: // INR A
        register_A = inr(register_A);
        break;
    case 0x3d: // DCR A
        register_A = dcr(register_A);
        break;
    case 0x3e: // MVI C, d8
        register_A = nextByte();
        break;
    case 0x3f: // CMC
        flag_C = !flag_C;
        break;

    case 0x40: // MOV B, B
        register_B = register_B;
        break;
    case 0x41: // MOV B, C
        register_B = register_C;
        break;
    case 0x42: // MOV B, D
        register_B = register_D;
        break;
    case 0x43: // MOV B, E
        register_B = register_E;
        break;
    case 0x44: // MOV B, H
        register_B = register_H;
        break;
    case 0x45: // MOV B, L
        register_B = register_L;
        break;
    case 0x46: // MOV B, M
        register_B = memory[register_HL];
        break;
    case 0x47: // MOV B, A
        register_B = register_A;
        break;

    case 0x48: // MOV C, B
        register_C = register_B;
        break;
    case 0x49: // MOV C, C
        register_C = register_C;
        break;
    case 0x4a: // MOV C, D
        register_C = register_D;
        break;
    case 0x4b: // MOV C, E
        register_C = register_E;
        break;
    case 0x4c: // MOV C, H
        register_C = register_H;
        break;
    case 0x4d: // MOV C, L
        register_C = register_L;
        break;
    case 0x4e: // MOV C, M
        register_C = memory[register_HL];
        break;
    case 0x4f: // MOV C, A
        register_C = register_A;
        break;

    case 0x50: // MOV D, B
        register_D = register_B;
        break;
    case 0x51: // MOV D, C
        register_D = register_C;
        break;
    case 0x52: // MOV D, D
        register_D = register_D;
        break;
    case 0x53: // MOV D, E
        register_D = register_E;
        break;
    case 0x54: // MOV D, H
        register_D = register_H;
        break;
    case 0x55: // MOV D, L
        register_D = register_L;
        break;
    case 0x56: // MOV D, M
        register_D = memory[register_HL];
        break;
    case 0x57: // MOV D, A
        register_D = register_A;
        break;

    case 0x58: // MOV E, B
        register_E = register_B;
        break;
    case 0x59: // MOV E, C
        register_E = register_C;
        break;
    case 0x5a: // MOV E, D
        register_E = register_D;
        break;
    case 0x5b: // MOV E, E
        register_E = register_E;
        break;
    case 0x5c: // MOV E, H
        register_E = register_H;
        break;
    case 0x5d: // MOV E, L
        register_E = register_L;
        break;
    case 0x5e: // MOV E, M
        register_E = memory[register_HL];
        break;
    case 0x5f: // MOV E, A
        register_E = register_A;
        break;

    case 0x60: // MOV H, B
        register_H = register_B;
        break;
    case 0x61: // MOV H, C
        register_H = register_C;
        break;
    case 0x62: // MOV H, D
        register_H = register_D;
        break;
    case 0x63: // MOV H, E
        register_H = register_E;
        break;
    case 0x64: // MOV H, H
        register_H = register_H;
        break;
    case 0x65: // MOV H, L
        register_H = register_L;
        break;
    case 0x66: // MOV H, M
        register_H = memory[register_HL];
        break;
    case 0x67: // MOV H, A
        register_H = register_A;
        break;

    case 0x68: // MOV L, B
        register_L = register_B;
        break;
    case 0x69: // MOV L, C
        register_L = register_C;
        break;
    case 0x6a: // MOV L, D
        register_L = register_D;
        break;
    case 0x6b: // MOV L, E
        register_L = register_E;
        break;
    case 0x6c: // MOV L, H
        register_L = register_H;
        break;
    case 0x6d: // MOV L, L
        register_L = register_L;
        break;
    case 0x6e: // MOV L, M
        register_L = memory[register_HL];
        break;
    case 0x6f: // MOV L, A
        register_L = register_A;
        break;

    case 0x70: // MOV M, B
        memory[register_HL] = register_B;
        break;
    case 0x71: // MOV M, C
        memory[register_HL] = register_C;
        break;
    case 0x72: // MOV M, D
        memory[register_HL] = register_D;
        break;
    case 0x73: // MOV M, E
        memory[register_HL] = register_E;
        break;
    case 0x74: // MOV M, H
        memory[register_HL] = register_H;
        break;
    case 0x75: // MOV M, L
        memory[register_HL] = register_L;
        break;
    case 0x76: // HLT
        halted = true;
        break;
    case 0x77: // MOV M, A
        memory[register_HL] = register_A;
        break;

    case 0x78: // MOV A, B
        register_A = register_B;
        break;
    case 0x79: // MOV A, C
        register_A = register_C;
        break;
    case 0x7a: // MOV A, D
        register_A = register_D;
        break;
    case 0x7b: // MOV A, E
        register_A = register_E;
        break;
    case 0x7c: // MOV A, H
        register_A = register_H;
        break;
    case 0x7d: // MOV A, L
        register_A = register_L;
        break;
    case 0x7e: // MOV A, M
        register_A = memory[register_HL];
        break;
    case 0x7f: // MOV A, A
        register_A = register_A;
        break;

    case 0x80: // ADD B
        add(register_B);
        break;
    case 0x81: // ADD C
        add(register_C);
        break;
    case 0x82: // ADD D
        add(register_D);
        break;
    case 0x83: // ADD E
        add(register_E);
        break;
    case 0x84: // ADD H
        add(register_H);
        break;
    case 0x85: // ADD L
        add(register_L);
        break;
    case 0x86: // ADD M
        add(memory[register_HL]);
        break;
    case 0x87: // ADD A
        add(register_A);
        break;

    case 0x88: // ADC B
        adc(register_B);
        break;
    case 0x89: // ADC C
        adc(register_C);
        break;
    case 0x8a: // ADC D
        adc(register_D);
        break;
    case 0x8b: // ADC E
        adc(register_E);
        break;
    case 0x8c: // ADC H
        adc(register_H);
        break;
    case 0x8d: // ADC L
        adc(register_L);
        break;
    case 0x8e: // ADC M
        adc(memory[register_HL]);
        break;
    case 0x8f: // ADC A
        adc(register_A);
        break;

    case 0x90: // SUB B
        sub(register_B);
        break;
    case 0x91: // SUB C
        sub(register_C);
        break;
    case 0x92: // SUB D
        sub(register_D);
        break;
    case 0x93: // SUB E
        sub(register_E);
        break;
    case 0x94: // SUB H
        sub(register_H);
        break;
    case 0x95: // SUB L
        sub(register_L);
        break;
    case 0x96: // SUB M
        sub(memory[register_HL]);
        break;
    case 0x97: // SUB A
        sub(register_A);
        break;

    case 0x98: // SBB B
        sbb(register_B);
        break;
    case 0x99: // SBB C
        sbb(register_C);
        break;
    case 0x9a: // SBB D
        sbb(register_D);
        break;
    case 0x9b: // SBB E
        sbb(register_E);
        break;
    case 0x9c: // SBB H
        sbb(register_H);
        break;
    case 0x9d: // SBB L
        sbb(register_L);
        break;
    case 0x9e: // SBB M
        sbb(memory[register_HL]);
        break;
    case 0x9f: // SBB A
        sbb(register_A);
        break;

    case 0xa0: // AND B
        ana(register_B);
        break;
    case 0xa1: // AND C
        ana(register_C);
        break;
    case 0xa2: // AND D
        ana(register_D);
        break;
    case 0xa3: // AND E
        ana(register_E);
        break;
    case 0xa4: // AND H
        ana(register_H);
        break;
    case 0xa5: // AND L
        ana(register_L);
        break;
    case 0xa6: // AND M
        ana(memory[register_HL]);
        break;
    case 0xa7: // AND A
        ana(register_A);
        break;

    case 0xa8: // XRA B
        xra(register_B);
        break;
    case 0xa9: // XRA C
        xra(register_C);
        break;
    case 0xaa: // XRA D
        xra(register_D);
        break;
    case 0xab: // XRA E
        xra(register_E);
        break;
    case 0xac: // XRA H
        xra(register_H);
        break;
    case 0xad: // XRA L
        xra(register_L);
        break;
    case 0xae: // XRA M
        xra(memory[register_HL]);
        break;
    case 0xaf: // XRA A
        xra(register_A);
        break;

    case 0xb0: // ORA B
        ora(register_B);
        break;
    case 0xb1: // ORA C
        ora(register_C);
        break;
    case 0xb2: // ORA D
        ora(register_D);
        break;
    case 0xb3: // ORA E
        ora(register_E);
        break;
    case 0xb4: // ORA H
        ora(register_H);
        break;
    case 0xb5: // ORA L
        ora(register_L);
        break;
    case 0xb6: // ORA M
        ora(memory[register_HL]);
        break;
    case 0xb7: // ORA A
        ora(register_A);
        break;

    case 0xb8: // CMP B
        cmp(register_B);
        break;
    case 0xb9: // CMP C
        cmp(register_C);
        break;
    case 0xba: // CMP D
        cmp(register_D);
        break;
    case 0xbb: // CMP E
        cmp(register_E);
        break;
    case 0xbc: // CMP H
        cmp(register_H);
        break;
    case 0xbd: // CMP L
        cmp(register_L);
        break;
    case 0xbe: // CMP M
        cmp(memory[register_HL]);
        break;
    case 0xbf: // CMP A
        cmp(register_A);
        break;

    case 0xc0: // RNZ
        ret(!flag_Z);
        break;
    case 0xc1: // POP B
        register_BC = pop();
        break;
    case 0xc2: // JNZ d15
        jmp(!flag_Z);
        break;
    case 0xc3: // JMP d16
        jmp(true);
        break;
    case 0xc4: // CNZ d16
        call(!flag_Z);
        break;
    case 0xc5: // PUSH B
        push(register_BC);
        break;
    case 0xc6: // ADI d8
        add(nextByte());
        break;

    case 0xc8: // RZ
        ret(flag_Z);
        break;
    case 0xc9: // RET
        ret(true);
        break;
    case 0xca: // JZ d16
        jmp(flag_Z);
        break;
    case 0xcc: // CZ d16
        call(flag_Z);
        break;
    case 0xcd: // CALL d16
        call(true);
        break;
    case 0xce: // ACI d8
        adc(nextByte());
        break;

    case 0xd0: // RNC
        ret(!flag_C);
        break;
    case 0xd1: // POP D
        register_DE = pop();
        break;
    case 0xd2: // JNC d16
        jmp(!flag_C);
        break;
    case 0xd3: // OUT d8
        out(nextByte(), register_A);
        break;
    case 0xd4: // CNC d16
        call(!flag_C);
        break;
    case 0xd5: // PUSH D
        push(register_DE);
        break;
    case 0xd6: // SUI d6
        sub(nextByte());
        break;

    case 0xd8: // RC d16
        ret(flag_C);
        break;
    case 0xda: // JC d16
        jmp(flag_C);
        break;
    case 0xdb: // IN d8
        register_A = in(nextByte());
        break;
    case 0xdc: // CC d16
        call(flag_C);
        break;
    case 0xde: // SBI d16
        sbb(nextByte());
        break;

    case 0xe0: // RNZ
        ret(!flag_P);
        break;
    case 0xe1: // POP H
        register_HL = pop();
        break;
    case 0xe2: // JPE d16
        jmp(!flag_P);
        break;
    case 0xe3: // XTHL
        std::swap(register_L, memory[stack_pointer++]);
        std::swap(register_L, memory[stack_pointer--]);
        break;
    case 0xe4: // JPO d16
        call(!flag_P);
        break;
    case 0xe5: // PUSH H
        push(register_HL);
        break;
    case 0xe6: // ANI d8
        ana(nextByte());
        break;

    case 0xe8: // RPE
        ret(flag_P);
        break;
    case 0xe9: // PCHL
        program_counter = register_HL;
        break;
    case 0xea: // JPE d16
        jmp(flag_P);
        break;
    case 0xeb: // XCHG
        std::swap(register_HL, register_DE);
        break;
    case 0xec: // CPE d16
        call(flag_P);
        break;
    case 0xee: // XRI d8
        xra(nextByte());
        break;

    case 0xf0: // RP
        ret(!flag_S);
        break;
    case 0xf1: // POP PSW
        register_PSW = pop();
        loadFlags();
        break;
    case 0xf2: // JPE d16
        jmp(!flag_S);
        break;
    case 0xf3: // DI
        interrupts_enabled = false;
        break;
    case 0xf4: // CP d16
        call(!flag_S);
        break;
    case 0xf5: // PUSH PSW
        storeFlags();
        push(register_PSW);
        break;
    case 0xf6: // ORI d8
        ora(nextByte());
        break;

    case 0xf8: // RM
        ret(flag_S);
        break;
    case 0xf9: // SPHL
        stack_pointer = register_HL;
        break;
    case 0xfa: // JM d16
        jmp(flag_S);
        break;
    case 0xfb: // EI
        interrupts_enabled = true;
        break;
    case 0xfc: // CM d16
        call(flag_S);
        break;
    case 0xfe: // CPI d8
        cmp(nextByte());
        break;

    default:
        break; // not reachable
    }

    return 1; // TODO return real cycle counts
}

uint8_t Intel8080::nextByte() { return memory[program_counter++]; }

uint16_t Intel8080::nextWord() {
    uint8_t low = nextByte();
    uint8_t high = nextByte();
    return (high << 8) | low;
}

void Intel8080::push(uint16_t word) {
    memory[--stack_pointer] = word >> 8;
    memory[--stack_pointer] = word;
}

uint16_t Intel8080::pop() {
    uint8_t low = memory[stack_pointer++];
    uint8_t high = memory[stack_pointer++];
    return (high << 8) | low;
}

void Intel8080::updateZSP(const uint8_t result) {
    flag_S = result & 0x80;
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