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

private:
	bool hasProgram;		// Whether or not a program is loaded into memory

	uint8_t memory[4096];	// Memory
	uint8_t V[16];			// General purpose registers
	uint16_t I;				// Index register
	uint8_t delayTimer;		// Delay timer register
	uint8_t soundTimer;		// Sound timer register

	uint16_t PC;			// Program counter
	uint8_t SP;				// Stack pointer

	uint16_t stack[16];		// Stack
};

#endif // EMULATOR_HPP