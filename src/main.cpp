#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "system.h"

// All essential functionality is located in system.cpp, where main.cpp simply acts
// as an entry point and overview of the system.

std::vector<cmd> commands =
{
	{ "connect",		[](std::string args) { return; } },
	{ "read",			[](std::string args) { return; } },
	{ "write",			[](std::string args) { return; } },
	{ "log",			[](std::string args) { return; } },
	{ "mode",			[](std::string args) { return; }, {
		{ "standby",		[](std::string args) { return; } },
		{ "manual",			[](std::string args) { return; } },
		{ "auto",			[](std::string args) { return; } }
	}},
	{ "set",			[](std::string args) { return; }, {
		{ "pos",			[](std::string args) { return; } },
		{ "specs",			[](std::string args) { return; } },
	}}
};

// main function
int main()
{
	// initialize CLI
	init_cli();

	// main loop
	while (true)
	{
		cli_main();
	}

	// exit
	return 0;
}
