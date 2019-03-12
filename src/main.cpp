#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "system.h"

#define newcmd new cmd_container(

// All essential functionality is located in system.cpp, where main.cpp simply acts
// as an entry point and overview of the system.

cmd_container commands =
{
	{ "connect",		[](std::string args) { uart::connect(&args[0]); } },
	{ "read",			[](std::string args) { return; } },
	{ "write",			[](std::string args) { return; } },
	{ "log",			[](std::string args) { return; } },
	{ "mode",			[](std::string args) { return; }, new cmd_container({
		{ "standby",		[](std::string args) { return; } },
		{ "manual",			[](std::string args) { return; } },
		{ "auto",			[](std::string args) { return; } },
	})},
	{ "set",			[](std::string args) { return; }, newcmd{
		{ "mode",			[](std::string args) { return; }, newcmd{
			{ "standby",		[](std::string args) { return; } },
			{ "manual",			[](std::string args) { return; } },
			{ "auto",			[](std::string args) { return; } }
		})},
	})},
	{ "get",			[](std::string args) { return; } },
	{ "cls",			[](std::string args) { init_cli(); } },
};

// main function
int main()
{

	// initialize command line interface
	init_cli();
	
	// main loop
	while(true)
	{
		cli();
	}

	// exit
	return 0;
}
