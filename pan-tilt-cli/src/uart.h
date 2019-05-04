#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <windows.h>

extern "C"
{
#include "chksum.h"
}

//// Defines //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define UART_COM_PORT			"COM4"
#define UART_BAUD				CBR_115200
//#define UART_BAUD				921600
#define UART_BYTESIZE			8
#define UART_ACK_TIMEOUT		3000

#define UART_FRAME_TYPE_BITS	3
#define UART_FRAME_SIZE_BITS	5
#define UART_MAX_PAYLOAD_SIZE	31
#define UART_BUFFER_FLUSH_TH	256

//// Public Declarations [Interface] //////////////////////////////////////////////////////////////////////////////////////////////

namespace uart
{
	// public constructs

	struct UART_FRAME
	{
		uint8_t		type : 3; /// should match parse_byte();
		uint8_t		size : 5; /// ^
		std::vector
		<uint8_t>	payload;
		uint8_t		checksum;
	};

	enum UART_FRAME_FIELD
	{
		TYPE,
		SIZE,
		PAYLOAD,
		CHECKUM
	};

	// maximum of 8 elements (2^3)
	// should match across MCU & CLI
	enum UART_FRAME_TYPE
	{
		UART_DO,
		UART_GET,
		UART_SET,
		UART_ACK,
		UART_RESPONSE,
		UART_MSG,
		UART_STREAM,
		UART_SAMPLEDATA
	};

	// public methods

	void		connect(const char* com_port = UART_COM_PORT);
	void		disconnect();

	bool		send(UART_FRAME_TYPE type, std::vector<uint8_t>& data);
	bool		request(uint8_t* buffer);

	void		print_frame(UART_FRAME& frame);

	namespace buffer
	{
		bool	has_data();
		int		queue_size();
		void	flush();
	}

	// public members

	namespace reciever
	{
		extern std::function<void(UART_FRAME frame)> callback_ack;
		extern std::function<void(UART_FRAME frame)> callback_msg;
		extern std::function<void(UART_FRAME frame)> callback_stm;
		extern std::function<void(UART_FRAME frame)> callback_sam;
	}
}