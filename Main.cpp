#include <iostream>
#include <unistd.h>
#include "Emulator.hpp"

using namespace std;

int main()
{
	Emulator emu;
	emu.reset();
	emu.loadProgram("games/MAZE");
	for (;;)
	{
		emu.step();
		if (emu.getShouldDraw()) {
			emu.draw();
		}
		usleep(1E6);
	}

	return 0;
}