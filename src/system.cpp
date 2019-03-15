#include "system.h"

// 1.0.0 -> created project, cmd parser
// 1.0.1 -> UART + chksum, setup sys namespace

//// Constants ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//// Definitions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init_cli()
{
	system("cls");
	std::cout << "Pan-Tilt System 1.0.1\n\n";
}

void cli()
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
	parse_input(input);
}

void parse_input(std::string input)
{
	// split input delimited by spaces into vector of strings
	std::istringstream input_iss(input);
	std::string args = input;

	std::vector<std::string> input_split(std::istream_iterator<std::string>{input_iss}, std::istream_iterator<std::string>());

	auto search_tree = &commands;

	// iterate words of command string
	for (const auto& word : input_split)
	{
		// check if command exists
		if (!search_tree->has_command(word))
			break;

		// update remaining args
		args.erase(args.find(word), word.length());

		// check if command takes specified arguments
		if (search_tree->get_command(word)->has_children())
		{
			search_tree = search_tree->get_command(word)->child_cmds;
			continue;
		}
		// otherwise run command with unspecified arguments
		else
		{
			//std::cout << "\n";
			search_tree->get_command(word)->func(args);
			if ( word != "cls") { std::cout << "\n\n"; }
			return;
		}
	}

	// catch all exceptions here
	std::cout << "The command \"" << input << "\" is invalid.\n\n";
}

void sys::connect(std::string com_port)
{
	// configure callbacks
	uart::reciever::callback_ack = nullptr;
	uart::reciever::callback_msg = nullptr;
	uart::reciever::callback_stm = nullptr;

	// connect UART
	(com_port == "") ? uart::connect() : uart::connect(com_port.c_str());
}

void sys::write(std::string byte)
{
	std::vector<uint8_t> tx_data = { (uint8_t)std::stoi(byte) };
	uart::send(uart::UART_FRAME_TYPE::RAW, tx_data);
}

void test_func(uart::UART_FRAME frame)
{
	printf("Got frame with checksum: %u", frame.checksum);
}