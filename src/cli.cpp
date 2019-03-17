#include "cli.h"

namespace cli
{
	cmd_container commands;

	void parse_input(std::string input);
}

void cli::init(cli::cmd_container commands)
{
	cli::commands = commands;
	
	// enable ANSI by clearing console
	system("cls");
}

void cli::get_input()
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

	// parse input command
	cli::parse_input(input);
}

void cli::parse_input(std::string input)
{
	// split input delimited by spaces into vector of strings
	std::istringstream input_iss(input);
	std::string args = input;

	std::vector<std::string> input_split(std::istream_iterator<std::string>{input_iss}, std::istream_iterator<std::string>());

	auto search_tree = &cli::commands;

	// iterate words of command string
	for (const auto& word : input_split)
	{
		// check if command exists
		if (!search_tree->has_command(word))
			break;

		// update remaining args
		args.erase(args.find(word), word.length() + 1);

		// check if command takes specified arguments
		if (search_tree->get_command(word)->has_children())
		{
			search_tree = search_tree->get_command(word)->child_cmds;
			continue;
		}
		// otherwise run command with remaining args
		else
		{
			//std::cout << "\n";
			search_tree->get_command(word)->func(args);
			if (word != "cls") { std::cout << "\n"; }
			return;
		}
	}

	// catch all exceptions here
	std::cout << "The command \"" << input << "\" is invalid.\n\n";
}

void cli::log_insert(std::string line)
{
	// http://www.inwap.com/pdp10/ansicode.txt
	
	static int num_of_lines = 1;
	
	// save horizontal cursor position
	std::cout << "\033[s";

	// insert <X> number of lines (\033[<X>L)
	std::cout << "\033[" << num_of_lines + 1 << "L";

	// move cursor to beginning
	std::cout << "\033[G";

	// print text
	std::cout << line << "\n";
	//std::cout << "\033[1;31mbold red text\033[0m";

	// restore horizontal cursor position
	std::cout << "\033[u";
		
	// move down <X> number of lines
	std::cout << "\033[" << num_of_lines + 1 << "B";
	
}

void cli::msgbox(std::string msg, std::string title = "Information")
{
	std::thread t([&]() {
		MessageBoxA(NULL, msg.c_str(), title.c_str(), MB_OK);
	});

	t.detach();
}

void cli::log_reset(std::string line = "")
{
	static std::string default_line = line;

	system("cls");
	std::cout << default_line << std::endl;
}