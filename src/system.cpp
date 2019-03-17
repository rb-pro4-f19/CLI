#include "system.h"

//// Constants ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//// Definitions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace sys
{

}

void sys::connect(std::string com_port)
{
	// configure callbacks
	

	// MSG callback
	uart::reciever::callback_msg = [&](uart::UART_FRAME frame)
	{
		cli::msgbox(std::string(frame.payload.begin(), frame.payload.end()), "Message from MCU");
		cli::log_insert("MESSAGE: " + std::string(frame.payload.begin(), frame.payload.end()));
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