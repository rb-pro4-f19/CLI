#pragma once

#include <string>
#include <vector>

//// Defines //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define UART_COM_PORT			"COM3"
#define UART_BAUD				CBR_9600
#define UART_BYTESIZE			8

#define UART_FRAME_TYPE_BITS	3
#define UART_FRAME_SIZE_BITS	5
#define UART_MAX_PAYLOAD_SIZE	256

//// Public Declarations [Interface] //////////////////////////////////////////////////////////////////////////////////////////////

namespace uart
{
	// Public Constructs

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

	enum UART_FRAME_TYPE
	{
		CONNECT,
		RAW,
		GET,
		SET,
		ACK,
		RESPONSE,
		STREAM,
		MSG
	};

	// Public Members

	;

	// Public Methods

	//void		init(const char* com_port = UART_COM_PORT);
	void		connect(const char* com_port = UART_COM_PORT);
	void		disconnect();

	bool		send(UART_FRAME_TYPE type, std::vector<uint8_t>& data);
	bool		request(uint8_t* buffer);	

	namespace reciever
	{
		/*static void(*callback_ack)(UART_FRAME frame) = nullptr;
		static void(*callback_msg)(UART_FRAME frame) = nullptr;
		static void(*callback_stm)(UART_FRAME frame) = nullptr;*/

		static std::function<void(UART_FRAME frame)> callback_ack = nullptr;
		static std::function<void(UART_FRAME frame)> callback_msg = nullptr;
		static std::function<void(UART_FRAME frame)> callback_stm = nullptr;
	}
}