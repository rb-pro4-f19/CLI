#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <windows.h>

#include "uart.h"

//// Private Declarations /////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace uart
{

	// Private Members

	bool connected = false;
	UART_MODE status = UART_MODE::DISABLED;
	int timeout_ms = 50;
	const char* com_port = UART_COM_PORT;

	HANDLE	com_handler;
	COMSTAT	com_status;
	DWORD	com_errors;

	// Private Methods
	
	bool transmit(UART_FRAME frm);
	bool write_byte(uint8_t byte);
	bool write_array(std::vector<uint8_t> &data);

	// Sub Namespaces

	namespace listener
	{
		int	required_bytes	= 3;
		void (*callback)()	= nullptr;

		std::atomic<std::vector<uint8_t>> buffer_response;
		std::atomic<std::vector<uint8_t>> buffer_stream;
		std::atomic<std::vector<uint8_t>> buffer_msg;

		std::thread	thread;

		void	set_callback();
		void	enable();
		void	disable();

		void	worker();
	}
}

//// Method Definitions ///////////////////////////////////////////////////////////////////////////////////////////////////////////

//// ::uart methods

void uart::connect(const char* com_port)
{
	if (uart::connected)
	{
		printf("Connection already established.");
		return;
	}

	// set COM port
	uart::com_port = com_port;

	// create handler
	uart::com_handler = CreateFileA(	
		static_cast<LPCSTR>(uart::com_port),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	// initialize handler and check for errors
	if (uart::com_handler == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			printf("Handle could not be attached; the specified COM port is unavailable.");
		}
		else
		{
			printf("Connection unsuccesful.");
		}

		return;
	}

	// set serial connection parameters
	DCB com_parameters = { 0 };

	if (!GetCommState(uart::com_handler, &com_parameters))
	{
		printf("Connection failed; failed to retrieve current serial parameters.");
		return;
	}

	com_parameters.BaudRate		= CBR_9600;
	com_parameters.ByteSize		= 8;
	com_parameters.StopBits		= ONESTOPBIT;
	com_parameters.Parity		= NOPARITY;
	com_parameters.fDtrControl	= DTR_CONTROL_ENABLE;

	// verify parameters
	if (!SetCommState(uart::com_handler, &com_parameters))
	{
		printf("Connection failed; could not set serial port parameters.");
		return;
	}

	// update status
	uart::status = UART_MODE::MANUAL;
	uart::connected = true;

	// flush handler
	PurgeComm(uart::com_handler, PURGE_RXCLEAR | PURGE_TXCLEAR);

	printf("Connection established.");
}

void uart::disconnect()
{
	uart::status = UART_MODE::DISABLED;
	CloseHandle(uart::com_handler);
}

void uart::send(UART_FRAME frame)
{

}

bool uart::write_byte(uint8_t byte)
{
	uint8_t tx_bytes[] = { byte };
	DWORD transmitted_bytes;

	if (!uart::connected)
	{
		printf("No active connection.");
		return false;
	}

	if (!WriteFile(uart::com_handler, tx_bytes, 1, &transmitted_bytes, NULL))
	{
		printf("Error writing byte.");
		ClearCommError(uart::com_handler, &uart::com_errors, &uart::com_status);
		return false;
	}

	return true;
}

bool uart::write_array(std::vector<uint8_t> &data)
{

	bool tx_succes = false;

	for (const auto& byte : data)
	{
		tx_succes = uart::write_byte(byte);
	}

	return tx_succes;
}

//// ::buffer methods

bool uart::buffer::has_data()
{
	ClearCommError(uart::com_handler, &uart::com_errors, &uart::com_status);
	return com_status.cbInQue != 0;
}

int uart::buffer::queue()
{
	return 0;
}

void uart::buffer::flush()
{
	return;
}