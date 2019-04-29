#include "system.h"
#include "cli.h"

// All essential functionality is located in system.cpp, where main.cpp simply acts
// as an entry point and overview of the system.

// Changelog can be found here:
// https://github.com/rb-pro4-f19/CLI/blob/master/CHANGELOG.md

// main function
int main()
{
	// initialize CLI command table
	cli::init(
	{
		{ "connect",		[](std::string args) { sys::connect(args); } },
		{ "cls",			[](std::string args) { cli::log_reset(); } },
		{ "exit",			[](std::string args) { exit(0); } },

		{ "echo",			[](std::string args) { sys::echo(); } },

		{ "gui",			[](std::string args) { sys::gui(); } },

		{ "write",			[](std::string args) {}, subcmd{
			{ "byte",			[](std::string args) { sys::write_byte(args); }},
			{ "array",			[](std::string args) { sys::write_array(args); }},
			{ "spi",			[](std::string args) { return; }},
			/// ...
		})},

		{ "set",			[](std::string args) {}, subcmd{
			{ "mode",			[](std::string args) { sys::set_mode(args); }},
			{ "pos",			[](std::string args) { sys::set_pos(args); }},
			{ "pwm",			[](std::string args) { sys::set_pwm(args); }},
			{ "freq",			[](std::string args) { sys::set_freq(args); }},
			{ "gui",			[](std::string args) { sys::set_gui(args); }},
			{ "msg",			[](std::string args) { sys::set_msg(args); }},
			{ "pid",			[](std::string args) { sys::set_pid(args); }},
			{ "slew",			[](std::string args) { sys::set_slew(args); }},
			{ "bound",			[](std::string args) { sys::set_bound(args); }},
			/// ...
		})},

		{ "get",			[](std::string args) {}, subcmd{
			{ "enc",			[](std::string args) { sys::get_enc(args); }},
			{ "hal",			[](std::string args) { sys::get_hal(args); }},
			/// ...
		})},

		/// ...
	});

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
