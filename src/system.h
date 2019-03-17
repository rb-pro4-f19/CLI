#pragma once

#include <string>
#include <functional>

#include "uart.h"
#include "cli.h"

//// Declarations /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SYSTEM_VERSION "1.0.4"

// 1.0.0 -> created project, cmd parser
// 1.0.1 -> UART + chksum, setup sys namespace
// 1.0.2 -> Working UART, fixed console parser bug (" COM3")
// 1.0.3 -> Add sys namespace, add cli namespace
// 1.0.4 -> Fixed ACK callback bug, Fixed UART callback bugs (static public), add cli::msgbox, add echo() command <<

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
}