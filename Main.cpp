#include <iostream>
#include "Emulator.hpp"

using namespace std;

int main()
{
	Emulator emu;
	emu.reset();
	emu.loadProgram("games/PONG");
	for (int i = 0; i < 20; ++i)
	{
		emu.step();
	}

	return 0;
}