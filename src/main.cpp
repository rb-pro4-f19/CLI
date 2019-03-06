#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "system.h"

// All essential functionality is located in system.cpp, where main.cpp simply acts
// as an entry point and overview of the system.

// commands map
std::unordered_map<std::string, void(*)(std::string args)> main_commands = 
{
	{ "connect",		[](std::string args) { return; } },
	{ "read",			[](std::string args) { return; } },
	{ "write",			[](std::string args) { return; } },
	{ "log",			[](std::string args) { return; } },
	{ "mode",			[](std::string args) { return; } }
};

std::unordered_map<std::string, void(*)(std::string args)> mode_commands =
{
	{ "demo",			[](std::string args) { return; } },
	{ "free",			[](std::string args) { return; } },
	{ "auto",			[](std::string args) { return; } }
};

std::unordered_map<std::string, void(*)(std::string args)> func_commands =
{ // define functions per mode
	{ "init",			[](std::string args) { return; } },
	{ "cal",			[](std::string args) { return; } },
	{ "set_pos",		[](std::string args) { return; } },
	{ "get_pos",		[](std::string args) { return; } }
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
