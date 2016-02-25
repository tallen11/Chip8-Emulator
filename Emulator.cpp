#include "Emulator.hpp"
#include <fstream>
#include <iostream>

Emulator::Emulator()
{
	this->reset();
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

			this->hasProgram = true;
		} else {
			std::cout << "Read failed: read " << file.gcount() << " bytes" << std::endl;
		}

		file.close();
		delete [] buffer;
	}
}

void Emulator::step()
{

}