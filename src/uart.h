#include <string>
#include <vector>

//// Defines //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define UART_COM_PORT "COM3"

//// Public Declarations [Interface] //////////////////////////////////////////////////////////////////////////////////////////////

namespace uart
{
	// Public Constructs

	enum class UART_MODE
	{
		DISABLED,
		MANUAL,
		LISTENER
	};

	enum class UART_TYPE
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

	struct UART_FRAME
	{
		uint8_t		type : 4;
		uint8_t		size : 4;
		uint8_t*	payload;
		uint8_t		checksum;
	};

	// Public Members

	;

	// Public Methods

	//void		init(const char* com_port = UART_COM_PORT);
	void		connect(const char* com_port = UART_COM_PORT);
	void		disconnect();

	bool		send(UART_TYPE type, std::vector<uint8_t> &data);
	bool		request(uint8_t* buffer);

}