#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <thread>
#include <atomic>
#include <windows.h>

extern "C"
{
#include "chksum.h"
}

#include "uart.h"
//// Private Declarations /////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace uart
{

	// Private Members

	bool connected = false;
	int tx_timeout_ms = 50;
	int rx_timeout_ms = 50;
	const char* com_port = UART_COM_PORT;

	HANDLE	com_handler;
	COMSTAT	com_status;
	DWORD	com_errors;

	// Private Methods
	
	bool write_byte(uint8_t byte);
	bool write_array(std::vector<uint8_t>& data);
	bool read_byte(uint8_t& destination_byte);
	uint8_t parse_byte(uint8_t byte, UART_FRAME_FIELD field);

	// Sub Namespaces

	namespace buffer
	{
		bool	has_data();
		int		queue();
		void	flush();
	}

	namespace reciever
	{
		
		enum UART_RX_STATE
		{
			IDLE,
			LISTEN,
			RECIEVE,
			CONSTRUCT_FRAME,
			VALIDATE,
			EXCEPTION
		};

		std::atomic<bool> enabled = false;

		UART_RX_STATE	state = IDLE;
		UART_FRAME		frame;
		uint16_t		num_rx_bytes = 0;
		std::string		error_msg = "Unspecfified error.";
		std::thread*	thread;

		std::array
		<uint8_t, UART_MAX_PAYLOAD_SIZE> frame_data;

		void worker();
	}
}

//// Method Definitions ///////////////////////////////////////////////////////////////////////////////////////////////////////////

/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ::uart methods
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uart::connect(const char* com_port)
{
	if (uart::connected)
	{
		printf("Connection is already established.");
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
	uart::connected = true;

	// flush handler
	PurgeComm(uart::com_handler, PURGE_RXCLEAR | PURGE_TXCLEAR);

	// enable reciever
	uart::reciever::state = reciever::LISTEN;
	uart::reciever::enabled = true;
	uart::reciever::thread = new std::thread(&reciever::worker);

	// log success
	printf("Connection established on %s.", uart::com_port);
}

void uart::disconnect()
{
	// remove handler
	uart::connected = false;
	CloseHandle(uart::com_handler);

	// stop reciever thread
	uart::reciever::enabled = false;
	uart::reciever::thread->join();
	uart::reciever::state = reciever::IDLE;
	delete uart::reciever::thread;

	// log success
	printf("Connection closed.");
}

bool uart::send(UART_FRAME_TYPE type, std::vector<uint8_t>& data)
{
	// construct frame

	// generate checksum

	// send data

	// wait for acknowledge or timeout (try again?)

	// return
	return true;
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

bool uart::write_array(std::vector<uint8_t>& data)
{

	bool tx_succes = false;

	for (const auto& byte : data)
	{
		tx_succes = uart::write_byte(byte);
	}

	return tx_succes;
}

bool uart::read_byte(uint8_t& destination_byte)
{
	
	//destination_byte = 0b1100'1010;
	//return true; // TEST ONLY

	// read buffer	
	DWORD	num_bytes_read;
	uint8_t buffer[1];

	// try reading from handler
	if (ReadFile(uart::com_handler, buffer, 1, &num_bytes_read, NULL))
	{
		destination_byte = buffer[0];
		return true;
	}
	else
	{
		return false;
	}
}

uint8_t uart::parse_byte(uint8_t byte, UART_FRAME_FIELD field)
{
	switch (field)
	{
		case TYPE:
			return (byte & 0b1110'0000);

		case SIZE:
			return (byte & 0b0001'1111);

		default:
			return byte;
	}
}

/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ::buffer methods
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool uart::buffer::has_data()
{
	//return true; // TEST ONLY
	
	ClearCommError(uart::com_handler, &uart::com_errors, &uart::com_status);
	return (com_status.cbInQue != 0);
}

int uart::buffer::queue()
{
	return 0;
}

void uart::buffer::flush()
{
	PurgeComm(uart::com_handler, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	printf("Buffer was flushed.");
}

/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ::reciever methods
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uart::reciever::worker()
{
	while (reciever::enabled)
	{

		printf("\n");

		switch (reciever::state)
		{
			case IDLE:
			{
				break;
			}				
		
			case LISTEN:
			{
				// assuming that only maximum of one byte will reside in the UART FIFO
				// otherwise another buffer is needed

				// check if there is data
				if (!buffer::has_data()) { break; }

				// try reading data
				uint8_t rx_byte;
				if (!uart::read_byte(rx_byte))
				{
					reciever::error_msg = "READ ERROR: Could not read data from handler, frame construction aborted.";
					reciever::state = EXCEPTION;
					break;
				}

				// construct new empty frame
				reciever::frame = UART_FRAME
				{
					uart::parse_byte(rx_byte, TYPE),
					uart::parse_byte(rx_byte, SIZE),
					{},
					0
				};

				// reserve expected size for vector 
				reciever::frame.payload.reserve(reciever::frame.size);

				// begin recieving
				reciever::state = RECIEVE;
				break; /// could be omitted
			}

			case RECIEVE:
			{
				// check timeout
				if (false)
				{
					reciever::error_msg = "TIMEOUT: Did not recieve sufficient bytes for a full frame before timeout.";
					reciever::state = EXCEPTION;
					break;
				}

				// check if there is data
				if (!buffer::has_data()) { break; }

				// try reading data
				uint8_t rx_byte;
				if (!uart::read_byte(rx_byte))
				{
					reciever::error_msg = "READ ERROR: Could not read data from handler, frame construction aborted.";
					reciever::state = EXCEPTION;
					break;
				}

				// append read byte to frame payload
				if (reciever::frame.payload.size() != reciever::frame.size)
				{
					// payload
					reciever::frame.payload.push_back(rx_byte);
				}
				else
				{
					// checksum (last byte)
					reciever::frame.checksum = rx_byte;
					reciever::state = VALIDATE;
				}

				break;
			}

			case VALIDATE:
			{
				// constuct vector for checksum check (header + payload)
				std::vector<uint8_t> checksum_data = reciever::frame.payload;
				checksum_data.insert(checksum_data.begin(), ((reciever::frame.type << 5) | reciever::frame.size));
				
				// validate the checksum
				if (!chksum.val_8bit(checksum_data.data(), checksum_data.size(), reciever::frame.checksum))
				{
					reciever::error_msg = "CHECKSUM ERROR: The recieved data did not have the expected checksum.";
					reciever::state = EXCEPTION;
					break;
				}

				// select callback according to frame type
				void(*callback)(UART_FRAME frame) = nullptr;

				switch (reciever::frame.type)
				{
					case UART_FRAME_TYPE::MSG:
						
						callback = reciever::callback_msg;
						break;

					case UART_FRAME_TYPE::STREAM:

						callback = reciever::callback_stm;
						break;

					case UART_FRAME_TYPE::RESPONSE:
					case UART_FRAME_TYPE::ACK:

						callback = reciever::callback_ack;
						break;
				}

				// check callback
				if (callback == nullptr)
				{
					reciever::error_msg = "CALLBACK ERROR: No callback was specified or error in frame type.";
					reciever::state = EXCEPTION;
					break;
				}

				// perform callback on detached thread
				std::thread callback_thread(callback, reciever::frame);
				callback_thread.detach();

				// reset reciever
				reciever::state = LISTEN;
				break;
			}


			case EXCEPTION:
			{
				// log error
				printf(reciever::error_msg.c_str());
				printf("\n");
				
				// reset
				buffer::flush();
				reciever::state = LISTEN;
				break;
			}
		}
	}
}