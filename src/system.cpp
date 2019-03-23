#include "system.h"

//// Constants ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//// Definitions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace sys
{

}

void sys::connect(std::string com_port)
{
	// configure callbacks
	

	// MSG callback (by-value)
	uart::reciever::callback_msg = [=](uart::UART_FRAME frame)
	{
		cli::msgbox(std::string(frame.payload.begin(), frame.payload.end()), "Message from MCU");
		//cli::log_insert("MESSAGE: " + std::string(frame.payload.begin(), frame.payload.end()));
	};

	// STREAM callback
	uart::reciever::callback_stm = nullptr;

	// connect UART
	(com_port == "") ? uart::connect() : uart::connect(com_port.c_str());
}

void sys::write(std::string byte)
{
	if (byte == "")
	{
		printf("WRITE ERROR: No write data specified.\n");
		return;
	}

	std::vector<uint8_t> tx_data = { (uint8_t)std::stoi(byte) };
	uart::send(uart::UART_FRAME_TYPE::RAW, tx_data);
}

void sys::echo()
{
	std::vector<uint8_t> tx_data = { 0x00 };
	uart::send(uart::UART_FRAME_TYPE::GET, tx_data);
}

void sys::set_pwm(std::string& inpit_args)
{
	// split input delimited by spaces into vector of strings
	std::istringstream input_iss(inpit_args);
	std::string args = inpit_args;

	std::vector<std::string> input_split(std::istream_iterator<std::string>{input_iss}, std::istream_iterator<std::string>());

	if (!input_split.size() == 2)
	{
		return;
	}

	uint8_t motor = std::stoi(input_split[0]);
	uint8_t value = std::stoi(input_split[1]);

	std::vector<uint8_t> tx_data = { 0x01, motor, value };
	uart::send(uart::UART_FRAME_TYPE::SET, tx_data);
}

void sys::get_enc(std::string& inpit_args)
{
	// split input delimited by spaces into vector of strings
	std::istringstream input_iss(inpit_args);
	std::string args = inpit_args;

	std::vector<std::string> input_split(std::istream_iterator<std::string>{input_iss}, std::istream_iterator<std::string>());

	if (!input_split.size() == 1)
	{
		return;
	}

	uint8_t enc = std::stoi(input_split[0]);

	std::vector<uint8_t> tx_data = { 0x01, enc };
	uart::send(uart::UART_FRAME_TYPE::GET, tx_data);
}