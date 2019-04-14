#include <iostream>

#include "gui.h"

int main()
{
	
	gui::init();

	while (true)
	{
		// read shared memory into gui::data
		gui::read_shm();

		// hide cursor
		gui::hidecur();

		// draw GUI
		gui::draw();
	}

	std::cin.get();
	return 0;
}