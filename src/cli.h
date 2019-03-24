#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>
#include <thread>
#include <algorithm>

#include <windows.h>

#define subcmd			new cli::cmd_container(
#define cmd_func(x)		[](std::string args) { x }

#define strvec			std::vector<std::string>

namespace cli
{

	struct cmd;
	struct cmd_container;

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
		cmd_container() {};

		cmd_container(std::initializer_list<cmd> init)
			: commands(init) {};

		std::vector<cmd> commands = {};

		int size()
		{
			return commands.size();
		}

		bool has_command(std::string cmd_name)
		{
			return (std::any_of(this->commands.begin(), this->commands.end(), [&](cmd &cmd_element) { return cmd_element.name == cmd_name; }));
		}

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

	void init(cli::cmd_container commands);
	void get_input();

	void log_reset(const std::string& line = "");
	void log_insert(const std::string& line);

	void msgbox(const std::string msg, const std::string title);

	std::vector<std::string> split_str(std::string args);
}