#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <cstdint>
#include <string>

const static uint16_t MEMORY_END = 0xFFF;
const static uint16_t MEMORY_PROGRAM_START = 0x200;

class Emulator
{
public:
	Emulator();
	~Emulator();
	void reset();
	void loadProgram(std::string filename);
	void step();
	bool getShouldDraw();
	void draw();

private:
	bool hasProgram;		  // Whether or not a program is loaded into memory

	uint8_t memory[4096];	  // Memory
	uint8_t V[16];			  // General purpose registers
	uint16_t I;				  // Index register
	uint8_t delayTimer;		  // Delay timer register
	uint8_t soundTimer;		  // Sound timer register

	uint8_t display[64][32];  // Display data
	bool shouldDraw;		  // Whether or not screen needs to be drawn

	uint16_t PC;			  // Program counter
	uint8_t SP;				  // Stack pointer

	uint16_t stack[16];		  // Stack
	uint16_t opcode;		  // Current opcode

	void fetch();
	uint16_t getAddress();

	void (Emulator::*procs[16])();
	void p_0_baseProcs();
	void p_1_JP();
	void p_2_CALL();
	void p_3_SE();
	void p_4_SNE();
	void p_5_SE();
	void p_6_LD();
	void p_7_ADD();
	void p_8_procs();
	void p_9_SNE();
	void p_A_LDI();
	void p_B_JPV();
	void p_C_RND();
	void p_D_DRW();
	void p_E_procs();
	void p_F_procs();

	void (Emulator::*s_procs_8[9])();
	void s_8_LD();
	void s_8_OR();
	void s_8_AND();
	void s_8_XOR();
	void s_8_ADD();
	void s_8_SUB();
	void s_8_SHR();
	void s_8_SUBN();
	void s_8_SHL();
};

#endif // EMULATOR_HPP