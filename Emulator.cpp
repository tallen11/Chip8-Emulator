#include "Emulator.hpp"
#include <fstream>
#include <iostream>
#include <random>
#include <ncurses.h>

void p(std::string msg) {
	// std::cout << msg << std::endl;
}

Emulator::Emulator()
{
	this->reset();

	// Set main function pointers
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

	// Set sub procedure function pointers
	s_procs_8[0] = &Emulator::s_8_LD;
	s_procs_8[1] = &Emulator::s_8_OR;
	s_procs_8[2] = &Emulator::s_8_AND;
	s_procs_8[3] = &Emulator::s_8_XOR;
	s_procs_8[4] = &Emulator::s_8_ADD;
	s_procs_8[5] = &Emulator::s_8_SUB;
	s_procs_8[6] = &Emulator::s_8_SHR;
	s_procs_8[7] = &Emulator::s_8_SUBN;
	s_procs_8[8] = &Emulator::s_8_SHL;
}

Emulator::~Emulator()
{

}

void Emulator::reset()
{
	for (uint16_t x = 0; x < 64 * 32; ++x) {
		display[x] = 0;
	}

	for (uint16_t i = 0; i < MEMORY_END; ++i) {
		if (i < 80) {
			this->memory[i] = fontset[i];
		} else {
			this->memory[i] = 0;
		}
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
	this->shouldDraw = false;
	this->waiting = false;

	srand(time(NULL));
}

void Emulator::startDisplay()
{
	initscr();
	noecho();
	curs_set(FALSE);
	clear();
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
	if (!waiting) {
		this->PC += 2;
	}

	opcode = (instr1 << 8) | instr2;
}

uint16_t Emulator::getAddress()
{
	return this->opcode & 0x0FFF;
}

bool Emulator::getShouldDraw()
{
	return shouldDraw;
}

void Emulator::draw()
{
	for (uint8_t x = 0; x < 64; ++x) {
		for (uint8_t y = 0; y < 32; ++y) {
			mvaddch(y + 1, x + 1, display[x + y * 64] == 1 ? '#' : ' ');
		}
	}

	this->drawBorder();

	refresh();

	shouldDraw = false;
}

void Emulator::drawBorder()
{
	for (uint8_t x = 0; x < 66; ++x) {
		for (uint8_t y = 0; y < 34; ++y) {
			if (x == 0 || x == 65) {
				mvaddch(y, x, ACS_VLINE);
			}

			if (y == 0 || y == 33) {
				mvaddch(y, x, ACS_HLINE);
			}
		}
	}

	mvaddch(0, 0, ACS_ULCORNER);
	mvaddch(33, 0, ACS_LLCORNER);
	mvaddch(0, 65, ACS_URCORNER);
	mvaddch(33, 65, ACS_LRCORNER);
}

void Emulator::step()
{
	this->fetch();
	(this->*procs[(opcode & 0xF000) >> 12])();

	if (!waiting) {
		if (delayTimer > 0) {
			--delayTimer;
		}

		if (soundTimer > 0) {
			if (soundTimer == 1) {
				p("BUZZ");
			}

			--soundTimer;
		}
	}
}


#pragma mark - Procedures

void Emulator::p_0_baseProcs()
{
	if (opcode == 0x00E0) {
		p("CLR_0");
		for (uint16_t x = 0; x < 64 * 32; ++x) {
			display[x] = 0;
		}

		shouldDraw = true;
	} else if (opcode == 0x00EE) {
		p("RET_0");
		SP--;
		PC = stack[SP];
	} else {
		p("CALL_0");
		uint16_t addr = this->getAddress();
		stack[SP] = PC;
		SP++;
		PC = addr;
	}
}

void Emulator::p_1_JP()
{
	p("JUMP_1");
	PC = this->getAddress();
}

void Emulator::p_2_CALL()
{
	p("CALL_2");
	stack[SP] = PC;
	SP++;
	PC = this->getAddress();
}

void Emulator::p_3_SE()
{
	p("SE_3");
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	if (V[x] == kk) {
		PC += 2;
	}
}

void Emulator::p_4_SNE()
{
	p("SNE_4");
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	if (V[x] != kk) {
		PC += 2;
	}
}

void Emulator::p_5_SE()
{
	p("SE_5");
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	if (V[x] == V[y]) {
		PC += 2;
	}
}

void Emulator::p_6_LD()
{
	p("LD_6");
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	V[x] = kk;
}

void Emulator::p_7_ADD()
{
	p("ADD_7");
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	V[x] += kk;
}

void Emulator::p_8_procs()
{
	uint8_t index = opcode & 0x000F;
	if (index == 0xE) {
		(this->*s_procs_8[8])();
	} else {
		(this->*s_procs_8[index])();
	}
}

void Emulator::p_9_SNE()
{
	p("SNE_9");
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	if (V[x] != V[y]) {
		PC += 2;
	}
}

void Emulator::p_A_LDI()
{
	p("LDI_A");
	I = getAddress();
}

void Emulator::p_B_JPV()
{
	p("JPV_B");
	PC = getAddress() + V[0];
}

void Emulator::p_C_RND()
{
	p("RAND_C");
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	uint8_t rb = rand() % 256;
	V[x] = rb & kk;
}

void Emulator::p_D_DRW()
{
	p("SPRITE_D");
	uint8_t x = V[(opcode & 0x0F00) >> 8];
	uint8_t y = V[(opcode & 0x00F0) >> 4];
	uint8_t n = opcode & 0x000F;
	V[0xF] = 0;
	for (uint8_t i = 0; i < n; i++) {
		uint8_t spriteByte = memory[i + I];
		for (uint8_t bit = 0; bit < 8; ++bit) {
			if ((spriteByte & (0x80 >> bit)) != 0) {
				uint16_t xc = (x + bit) % 64;
				uint16_t yc = (y + i) % 32;
				if (display[xc + yc * 64] == 1)
					V[0xF] = 1;
				
				display[xc + yc * 64] ^= 1;
			}
		}
	}

	shouldDraw = true;
}

void Emulator::p_E_procs()
{
	uint8_t least = opcode & 0x00FF;
	// uint8_t x = (opcode & 0x0F00) >> 8;
	if (least == 0x9E) {
		p("PRESSED_E");
		// if x is pressed, PC += 2
	} else if (least == 0xA1) {
		p("NOT PRESSED_E");
		// if x is NOT pressed, PC += 2
	}
}

void Emulator::p_F_procs()
{
	// p("Subprocedures_F");
	uint8_t least = opcode & 0x00FF;
	uint8_t x = (opcode & 0x0F00) >> 8;
	switch (least) {
		case 0x07: {
			p("LD_RDT_F");
			V[x] = delayTimer;
			break;
		}

		case 0x0A: {
			p("Wants key press!");
			waiting = true;
			break;
		}

		case 0x15: {
			p("LD_DT_F");
			delayTimer = V[x];
			break;
		}

		case 0x18: {
			p("LD_ST_F");
			soundTimer = V[x];
			break;
		}

		case 0x1E: {
			p("ADD_F");
			I += V[x];
			break;
		}

		case 0x29: {
			p("FONT_F");
			I = V[x] * 0x5;
			break;
		}

		case 0x33: {
			p("LD_BCD_F");
			uint16_t val = V[x];
			memory[I] = val / 100;
			memory[I+1] = (val / 10) % 10; // (val - memory[I] * 100) / 10;
			memory[I+2] = (val % 100) % 10; //val - memory[I] * 100 - memory[I+1] * 10;
			break;
		}

		case 0x55: {
			p("LD_M_F");
			for (uint8_t i = 0; i < x; i++) {
				memory[i + I] = V[i];
			}

			// I += x + 1;

			break;
		}

		case 0x65: {
			p("LD_R_F");
			for (uint8_t i = 0; i < x; i++) {
				V[i] = memory[i + I];
			}

			// I += x + 1;

			break;
		}
	}
}


#pragma mark - Subprocedures_8

void Emulator::s_8_LD()
{
	p("LD_8");
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	V[x] = V[y];
}

void Emulator::s_8_OR()
{
	p("OR_8");
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	V[x] = V[x] | V[y];
}

void Emulator::s_8_AND()
{
	p("AND_8");
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	V[x] = V[x] & V[y];
}

void Emulator::s_8_XOR()
{
	p("XOR_8");
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	V[x] = V[x] ^ V[y];
}

void Emulator::s_8_ADD()
{
	p("ADD_8");
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	uint16_t sum = V[x] + V[y];
	uint8_t truncSum = sum & 0x00FF;
	V[x] = truncSum;
	if (sum != truncSum) {
		V[0xF] = 1;
	}
}

void Emulator::s_8_SUB()
{
	p("SUB_8");
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	V[0xF] = V[x] > V[y] ? 1 : 0;
	V[x] = V[x] - V[y];
}

void Emulator::s_8_SHR()
{
	p("SHR_8");
	uint8_t x = (opcode & 0x0F00) >> 8;
	// uint8_t y = (opcode & 0x00F0) >> 4;
	V[0xF] = V[x] & 1;
	V[x] /= 2;
}

void Emulator::s_8_SUBN()
{
	p("SUBN_8");
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	V[0xF] = V[y] > V[x] ? 1 : 0;
	V[x] = V[y] - V[x];
}

void Emulator::s_8_SHL()
{
	p("SHL_8");
	uint8_t x = (opcode & 0x0F00) >> 8;
	// uint8_t y = (opcode & 0x00F0) >> 4;
	V[0xF] = V[x] & 128;
	V[x] *= 2;
}
