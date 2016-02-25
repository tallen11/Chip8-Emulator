#include <iostream>
#include "Emulator.hpp"

using namespace std;

int main()
{
	Emulator emu;
	emu.reset();
	emu.loadProgram("games/PONG");

	return 0;
}