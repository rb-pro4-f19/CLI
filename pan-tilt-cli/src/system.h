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

	// should match across MCU & CLI
	enum CMD_ID
	{
		// UART_GET
		GET_ECHO,
		GET_ENC,
		GET_HAL,

		// UART_SET
		SET_MODE,
		SET_POS,
		SET_GUI,
		SET_MSG,
		SET_PWM,
		SET_FREQ,
		SET_SLEW,
		SET_BOUND,
		SET_PID,
	};

	// should match across MCU & CLI
	enum class PID_PARAM
	{
		PID_KP,		// proportional gain
		PID_KI,		// integrator gain
		PID_KD,		// derivative gain
		PID_N,		// filter constant
		PID_I,		// gain index
	};

	// public methods

	void connect(std::string com_port);
	
	void gui();
	void stream_handler(uart::UART_FRAME frame);

	void write_byte(std::string byte);
	void write_array(std::string args);
	void write_spi(std::string args);

	void echo();

	void get_enc(std::string args);
	void get_hal(std::string args);

	void set_mode(std::string args);
	void set_pos(std::string args);
	void set_pos_single(uint8_t mot_id, float value);
	void set_gui(std::string args);
	void set_msg(std::string args);

	void set_pwm(std::string args);
	void set_freq(std::string args);
	void set_slew(std::string args);
	void set_bound(std::string args);

	void set_pid(std::string args);
	void set_pid_param(uint8_t pid_id, PID_PARAM pid_param, float value);

}
