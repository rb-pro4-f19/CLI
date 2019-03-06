#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <windows.h>

#include "system.h"

//// Constants ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr int 	KEY_WAIT				= 30;											// Duration between key presses
constexpr int 	KEY_DURATION_MOVE		= 250;											// Duration of a simulated key press (moving)
constexpr int 	KEY_DURATION_PRIMARY	= 25;											// Duration of a simulated key press (primary)

//// Definitions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Initialize the CLI
void init_cli()
{
	system("cls");
	//std::cout << "Ear Rape Simulator 1.1.0\n\n";
}

// ...
void cli_main()
{
	// show >
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	std::cout << "> ";

	//	receive input
	std::string input;
	std::getline(std::cin, input);

	// check input length
	if (input.size() == 0)
	{
		return;
	}

	// execute input command
	execute_cmd(input);
}

void execute_cmd(std::string input)
{
	// parse cmd
	std::string cmd = input.substr(0, input.find(' '));
	std::string args = "";

	// check if command exists
	if (command_map.count(cmd) == 0)
	{
		std::cout << "The command \"" << cmd << "\" is invalid.\n";
		return;
	}

	// check whether command has arguments; parse args if true
	if (input.find(' ') != -1)
	{
		args = input.substr(cmd.length() + 1);
	}

	// call appropriate function from map
	command_map[cmd](args);
}