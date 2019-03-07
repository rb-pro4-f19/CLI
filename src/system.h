#pragma once
#include <thread>

//// Declarations /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Variables
extern std::unordered_map<std::string, void(*)(std::string args)> command_map;

// Modes

struct cmd
{
	std::string name;
	void(*func)(std::string args) = nullptr;
	std::vector<cmd> args = {};
	std::string help = "No information available.";
};

enum class SYSTEM_MODE 
{
	STANDBY,
	IDLE,
	RUN

};

enum class OPERATION_MODE
{
	STANDBY,
	IDLE,
	RUN
};

// Methods
void execute_cmd(std::string input);
void init_cli();
void cli_main();
