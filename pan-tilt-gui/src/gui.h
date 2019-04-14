#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <windows.h>

#include "..\..\shm_def.h"

//// Defines //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//// Public Declarations [Interface] //////////////////////////////////////////////////////////////////////////////////////////////

namespace gui
{
	// public methods
	void init();
	void draw();
	void hidecur();

	void read_shm();

	// public members
	extern GUI_DATA data;
}