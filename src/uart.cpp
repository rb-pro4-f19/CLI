#include <thread>

#include "uart.h"

//// Private Declarations /////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace uart
{

	// Private Members
	bool connected = false;
	UART_MODE status = UART_MODE::DISABLED;

	// Private Methods
	namespace buffer
	{
		bool	has_data();
		int		queue();
		void	flush();
	}

	namespace listener
	{
		int	required_bytes	= 3;
		void (*callback)()	= nullptr;
		std::thread	thread;
		
		void	set_callback();
		void	enable();
		void	disable();

		void	worker();
	}
}

//// Method Definitions ///////////////////////////////////////////////////////////////////////////////////////////////////////////

void uart::connect()
{
	uart::connected = true;
	return;
}

bool uart::buffer::has_data()
{
	return true;
}