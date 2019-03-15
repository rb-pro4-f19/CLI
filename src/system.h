#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>
#include <unordered_map>
#include <windows.h>
#include <thread>
#include <algorithm>

#include "uart.h"

//// Declarations /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace sys
{
	void connect(std::string com_port);
}

struct cmd;
struct cmd_container;

extern cmd_container commands;

enum class SYSTEM_MODE;
enum class OPERATION_MODE;

void parse_input(std::string input);
void init_cli();
void cli();

void test_func(uart::UART_FRAME frame);

struct cmd
{
	std::string name;
	void(*func)(std::string args) = nullptr;
	cmd_container* child_cmds = nullptr;
	std::string help = "No information available.";

	bool has_children() { return this->child_cmds != nullptr; }
};

struct cmd_container
{
	cmd_container(std::initializer_list<cmd> init)
		: commands(init) {};

	std::vector<cmd> commands = {};

	int	 size() { return commands.size(); }

	bool has_command(std::string cmd_name)
	{
		return (std::any_of(this->commands.begin(), this->commands.end(), [&](cmd &cmd_element) { return cmd_element.name == cmd_name; }));
	}

	bool has_children(std::string cmd_name) { return true; }

	cmd* get_command(std::string cmd_name)
	{
		auto it = std::find_if(this->commands.begin(), this->commands.end(), [&](cmd &cmd_element) { return cmd_element.name == cmd_name; });
		return &(*it);		
	} // operator overload -> cmd_container("cmd");

	cmd* operator [] (std::string cmd_name)
	{
		return this->get_command(cmd_name);
	}
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
