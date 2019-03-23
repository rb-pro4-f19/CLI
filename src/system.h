#pragma once

#include <string>
#include <functional>

#include "uart.h"
#include "cli.h"

//// Declarations /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SYSTEM_VERSION "1.4.0"

//// Declarations /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace sys
{

	enum class SYSTEM_MODE
	{
		STANDBY,
		IDLE,
		RUN
	};

	void connect(std::string com_port);
	void write(std::string byte);

	void echo();
	void set_pwm(std::string& args);
	void get_enc(std::string& args);
}