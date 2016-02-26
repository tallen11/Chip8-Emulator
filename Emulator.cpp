#include "Emulator.hpp"
#include <fstream>
#include <iostream>

Emulator::Emulator()
{
	this->reset();
	procs[0] = &Emulator::p_0_baseProcs;
	procs[1] = &Emulator::p_1_JP;
	procs[2] = &Emulator::p_2_CALL;
	procs[3] = &Emulator::p_3_SE;
	procs[4] = &Emulator::p_4_SNE;
	procs[5] = &Emulator::p_5_SE;
	procs[6] = &Emulator::p_6_LD;
	procs[7] = &Emulator::p_7_ADD;
	procs[8] = &Emulator::p_8_procs;
	procs[9] = &Emulator::p_9_SNE;
	procs[10] = &Emulator::p_A_LDI;
	procs[11] = &Emulator::p_B_JPV;
	procs[12] = &Emulator::p_C_RND;
	procs[13] = &Emulator::p_D_DRW;
	procs[14] = &Emulator::p_E_procs;
	procs[15] = &Emulator::p_F_procs;
}

Emulator::~Emulator()
{

}

void Emulator::reset()
{
	for (uint16_t i = 0; i < MEMORY_PROGRAM_START; ++i) {
		this->memory[i] = 0;
	}

	for (uint8_t i = 0; i < 16; ++i) {
		this->V[i] = 0;
	}

	for (uint8_t i = 0; i < 16; ++i) {
		this->stack[i] = 0;
	}

	this->hasProgram = false;
	this->I = 0;
	this->delayTimer = 0;
	this->soundTimer = 0;
	this->PC = 0;
	this->SP = 0;
	this->opcode = 0;
}

void Emulator::loadProgram(std::string filename)
{
	std::ifstream file(filename, std::ifstream::binary);
	if (file) {
		file.seekg(0, file.end);
		uint16_t length = file.tellg();
		file.seekg(0, file.beg);

		char *buffer = new char[length];
		std::cout << "Reading " << length << " bytes..." << std::endl;
		file.read(buffer, length);
		if (file) {
			std::cout << "Read successful! Copying..." << std::endl;
			for (uint16_t i = 0; i < length; ++i) {
				this->memory[i + MEMORY_PROGRAM_START] = buffer[i];
			}

			this->PC = MEMORY_PROGRAM_START;
			this->hasProgram = true;
		} else {
			std::cout << "Read failed: read " << file.gcount() << " bytes" << std::endl;
		}

		file.close();
		delete [] buffer;
	}
}

void Emulator::fetch()
{
	uint8_t instr1 = this->memory[this->PC];
	uint8_t instr2 = this->memory[this->PC + 1];
	this->PC += 2;
	opcode = (instr1 << 8) | instr2;
}

uint16_t Emulator::getAddress()
{
	return this->opcode & 0x0FFF;
}

void Emulator::step()
{
	this->fetch();
	(this->*procs[(opcode & 0xF000) >> 12])();
}


#pragma mark - Procedures

void Emulator::p_0_baseProcs()
{
	if (opcode == 0x00E0) {
		// Clear the display
		std::cout << "Clear display!" << std::endl;
	} else if (opcode == 0x00EE) {
		// RETurn from subroutine
		PC = stack[SP];
		SP--;
	} else {
		// SYS call
		uint16_t addr = this->getAddress();
		stack[SP] = PC;
		SP++;
		PC = addr;
	}
}

void Emulator::p_1_JP()
{
	PC = this->getAddress();
}

void Emulator::p_2_CALL()
{
	stack[SP] = PC;
	SP++;
	PC = this->getAddress();
}

void Emulator::p_3_SE()
{
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint16_t kk = opcode & 0x00FF;
	if (V[x] == kk) {
		PC += 2;
	}
}

void Emulator::p_4_SNE()
{
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint16_t kk = opcode & 0x00FF;
	if (V[x] != kk) {
		PC += 2;
	}
}

void Emulator::p_5_SE()
{
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	if (V[x] == V[y]) {
		PC += 2;
	}
}

void Emulator::p_6_LD()
{
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint16_t kk = opcode & 0x00FF;
	V[x] = kk;
}

void Emulator::p_7_ADD()
{
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint16_t kk = opcode & 0x00FF;
	V[x] += kk;
}

void Emulator::p_8_procs()
{

}

void Emulator::p_9_SNE()
{

}

void Emulator::p_A_LDI()
{

}

void Emulator::p_B_JPV()
{

}

void Emulator::p_C_RND()
{

}

void Emulator::p_D_DRW()
{

}

void Emulator::p_E_procs()
{

}

void Emulator::p_F_procs()
{

}
