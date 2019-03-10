#include <vector>


//// Public Declarations [Interface] //////////////////////////////////////////////////////////////////////////////////////////////

namespace uart
{
	// Public Members

	enum class UART_MODE
	{
		DISABLED,
		MANUAL,
		LISTENER
	};

	struct UART_FRAME
	{
		uint8_t		type : 4;
		uint8_t		size : 4;
		uint8_t*	payload;
		uint8_t		checksum;
	};

	// Public Methods

	void		init();
	void		connect();
	void		disconnect();

	void		write(uint8_t data[]);
	void		write_byte(uint8_t byte);
	
	uint8_t*	read_all();
	uint8_t		read_byte();
	uint8_t*	read_bytes(uint8_t num_of_bytes);

	bool		send(UART_FRAME frame);
	bool		request(uint8_t* buffer);

	namespace listener
	{
		void	enable();
		void	disable();
	}

}