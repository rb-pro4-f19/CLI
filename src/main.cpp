#include "system.h"
#include "cli.h"

// All essential functionality is located in system.cpp, where main.cpp simply acts
// as an entry point and overview of the system.

// Changelog can be found here:
// https://github.com/rb-pro4-f19/CLI/blob/master/CHANGELOG.md

// main function
int main()
{
	// initialize CLI
	cli::init(
	{
		{ "connect",		[](std::string args) { sys::connect(args); } },
		{ "echo",			[](std::string args) { sys::echo(); } },
		{ "write",			[](std::string args) { sys::write(args); } },
		{ "set",			[](std::string args) { return; }, newcmd{
			{ "mode",			[](std::string args) { return; }, newcmd{
				{ "standby",		[](std::string args) { return; } },
				{ "manual",			[](std::string args) { return; } },
				{ "auto",			[](std::string args) { return; } }
			})},
		})},
		{ "get",			[](std::string args) { return; } },
		{ "cls",			[](std::string args) { cli::log_reset(); } },
		{ "exit",			[](std::string args) { exit(0); } },
	});

	// test of cli::log_insert();
	/*std::thread t([](){
		while (true)
		{
			for (int i = 0; i < 1000000000; i++);
			cli::log_insert("This is a test line.");
		}
	});*/

	// log system information
	cli::log_reset("Pan-Tilt System " + std::string(SYSTEM_VERSION));

	// main loop
	while (true)
	{
		cli::get_input();
	}

	// exit
	return 0;
}
