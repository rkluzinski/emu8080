#ifndef INTEL_8080_H
#define INTEL_8080_H

#include <array>
#include <cstdint>
#include <functional>
#include <string>

class Intel8080 {
  public:
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
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
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    union {
        struct {
            uint8_t register_B;
            uint8_t register_C;
        };
        uint16_t register_BC;
    };
    union {
        struct {
            uint8_t register_D;
            uint8_t register_E;
        };
        uint16_t register_DE;
    };
    union {
        struct {
            uint8_t register_H;
            uint8_t register_L;
        };
        uint16_t register_HL;
    };
    union {
        struct {
            uint8_t register_A;
            uint8_t flags;
        };
        uint16_t register_PSW;
    };
#else
#error "Host machine endianess not defined"
#endif

    bool flag_S;
    bool flag_Z;
    bool flag_A;
    bool flag_P;
    bool flag_C;

    uint16_t stack_pointer = 0x0000;
    uint16_t program_counter = 0x0000;

    std::array<uint8_t, 0x10000> memory;

    // Callbacks for interacting with I/O devices
    std::function<uint8_t(uint8_t)> in;
    std::function<void(uint8_t, uint8_t)> out;

    bool halted = false;
    bool interrupts_enabled = true;

    /**
     * Execute until the CPU halts
     * Parameters:
     *     cycles (optional) - The target cycles to execute
     * Returns: The number of clock cycles executed
     */
    std::size_t execute();
    std::size_t execute(std::size_t target_cycles);

	/**
	 * Calls the given interrupt service routines
	 * Parameter:
	 *     isr - The number of the given ISR (0-7)
	 */
	void interrupt(const int isr);

    /**
     * Reset the CPU's state
     * - Unhalts CPU
     * - Enables interrupts
     * - Resets PC and SP to 0
     * - Does not affect registers
     */
    void reset();

    /**
     * Execute the next instruction
     * Returns: How many clock cycles the CPU executed
     */
    std::size_t step();

  private:
	// interrupt service routine vector
	static const std::array<uint16_t, 8> interrupt_vector;

	// instruction timings
	static const std::array<std::size_t, 256> instruction_timing;

    // immediate data operations
    uint8_t nextByte();
    uint16_t nextWord();

    // stack operations
    void push(const uint16_t word);
    uint16_t pop();

    // flag operations
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