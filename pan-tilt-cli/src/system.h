#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <functional>

#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <windows.h>

#include "..\..\shm_def.h"
#include "uart.h"
#include "cli.h"


//// Defines //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SYSTEM_VERSION "1.7.0"

//// Public Declarations [Interface] //////////////////////////////////////////////////////////////////////////////////////////////

namespace sys
{

	// constructs

	enum class SYSTEM_MODE
	{
		STANDBY,
		IDLE,
		RUN
	};

	// public methods

	void connect(std::string com_port);
	
	void gui();
	void stream_handler(uart::UART_FRAME frame);

	void write_byte(std::string byte);
	void write_array(std::string args);
	void write_spi(std::string args);

	void echo();
	void set_mode(std::string args);
	void set_gui(std::string args);
	void set_msg(std::string args);
	void set_pwm(std::string args);
	void set_freq(std::string args);

	void get_enc(std::string args);
	void get_hal(std::string args);
}
