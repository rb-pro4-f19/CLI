#include "system.h"

//// Private Declarations /////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace sys
{
	GUI_DATA gui_data;

	DWORD	gPidToFind = 0;
	HWND	gTargetWindowHwnd = NULL;
	BOOL CALLBACK myWNDENUMPROC(HWND hwCurHwnd, LPARAM lpMylp);

	enum STREAM_FRAME_SIZE
	{
		STREAM_DAT_FRAME = 1,
		STREAM_CMD_FRAME = 2
	};

	bool gui_open = false;

	namespace shm
	{	
		HANDLE h_map_file;
		LPTSTR data_buf;

		void open();
		void close();
		void write(void* data, int size);
	}
}

//// Method Definitions ///////////////////////////////////////////////////////////////////////////////////////////////////////////

void sys::connect(std::string com_port)
{
	// MSG callback (by-value)
	uart::reciever::callback_msg = [=](uart::UART_FRAME frame)
	{
		auto msg_str = std::string(frame.payload.begin(), frame.payload.end());
		cli::msgbox(msg_str, "Message from MCU");
		//cli::log_insert("MESSAGE: " + std::string(frame.payload.begin(), frame.payload.end()));
	};

	// STREAM callback
	uart::reciever::callback_stm = nullptr;

	// connect UART to specified or default COM port
	(com_port == "") ? uart::connect() : uart::connect(com_port.c_str());
}

void sys::gui()
{
	// check if allready open
	if (sys::gui_open) { return; }
	
	// init GUI data
	sys::gui_data.mode = 0;
	sys::gui_data.mot0 = {  12, 80, 0, 0.0f, false, 0, 0, 10.0f, 0.0f, 0.0f };
	sys::gui_data.mot1 = { 113, 80, 0, 0.0f, false, 0, 0, 20.0f, 0.0f, 0.0f };

	// open shared memory
	sys::shm::open();

	// initial write to shared memory
	sys::shm::write(&sys::gui_data, sizeof(sys::gui_data));

	// initialize MCU callback
	uart::reciever::callback_stm = &sys::stream_handler;

	// run process
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// get current path
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	auto exe_dir = std::string(buffer).substr(0, std::string(buffer).find_last_of("\\/"));

	// set path og GUI exe file
	auto gui_dir = std::string(exe_dir + "\\gui.exe");

	// start the program up
	CreateProcessA(
		gui_dir.c_str(),	// the path
		LPSTR(""),			// command line
		NULL,				// process handle not inheritable
		NULL,				// thread handle not inheritable
		FALSE,				// set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,	// no creation flags
		NULL,				// use parent's environment block
		NULL,				// use parent's starting directory 
		&si,				// pointer to STARTUPINFO structure
		&pi					// pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);

	// sleep a bit
	Sleep(100);
	
	// relocate GUI window
	constexpr auto gui_pos_x = 70;
	constexpr auto gui_pos_y = 70;
	constexpr auto gui_dim_w = 570;
	constexpr auto gui_dim_h = 405;

	gPidToFind = pi.dwProcessId;
	EnumWindows(sys::myWNDENUMPROC, NULL);

	HWND gui_hwnd = sys::gTargetWindowHwnd;
	auto gui_dpi = GetDpiForWindow(gui_hwnd);

	MoveWindow(
		gui_hwnd,
		MulDiv(gui_pos_x, gui_dpi, 96),
		MulDiv(gui_pos_y, gui_dpi, 96),
		MulDiv(gui_dim_w, gui_dpi, 96),
		MulDiv(gui_dim_h, gui_dpi, 96),
		TRUE
	);

	// close process and thread handles
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	// sleep a bit
	Sleep(100);

	// relocate current console window
	constexpr auto con_pos_x = gui_pos_x;
	constexpr auto con_pos_y = gui_pos_y + gui_dim_h + 5;
	constexpr auto con_dim_w = gui_dim_w;
	constexpr auto con_dim_h = 300;

	HWND con_hwnd	= GetConsoleWindow();
	auto con_dpi	= GetDpiForWindow(con_hwnd);
	
	MoveWindow(
		con_hwnd,
		MulDiv(con_pos_x, con_dpi, 96),
		MulDiv(con_pos_y, con_dpi, 96),
		MulDiv(con_dim_w, con_dpi, 96),
		MulDiv(con_dim_h, con_dpi, 96),
		TRUE
	);

	// set focus to console
	SetForegroundWindow(con_hwnd);

	// update variable
	sys::gui_open = true;

	// disable MCU message logging
	sys::set_msg("0");

	// enable MCU GUI data transmission
	sys::set_gui("1");
}

BOOL CALLBACK sys::myWNDENUMPROC(HWND hwCurHwnd, LPARAM lpMylp)
{
	// helper method to find process HWND from HANDLE

	DWORD dwCurPid = 0;
	char lpWndText[256];
	char lpWndClass[256];

	::GetWindowText(hwCurHwnd, lpWndText, sizeof(lpWndText));
	::GetClassName(hwCurHwnd, lpWndClass, sizeof(lpWndClass));

	GetWindowThreadProcessId(hwCurHwnd, &dwCurPid);

	if (dwCurPid == sys::gPidToFind)
	{
		sys::gTargetWindowHwnd = hwCurHwnd;
		return FALSE;
	}

	return TRUE;
}

void sys::stream_handler(uart::UART_FRAME frame)
{
	constexpr auto expected_num_bytes = sizeof(sys::gui_data);

	static GUI_DATA	rx_data;
	static uint8_t*	rx_data_ptr;
	static unsigned rx_num_bytes = 0;

	// check if reset frame recieved
	if (frame.size == STREAM_CMD_FRAME)
	{
		rx_num_bytes = 0;
		memset(&rx_data, 0, sizeof(rx_data));
		rx_data_ptr = (uint8_t*)(&rx_data);
		return;
	}

	// check if data frame; throw otherwise
	if (frame.size != STREAM_DAT_FRAME) { return; }

	// safety checks
	if (rx_num_bytes >= expected_num_bytes || rx_data_ptr == nullptr) { return; }

	// append data to struct
	*rx_data_ptr++ = frame.payload[0];

	// check if enough bytes recieved
	if (++rx_num_bytes != expected_num_bytes) { return; }

	// write data to shared memory (by-ref)
	sys::gui_data = rx_data;
	sys::shm::write(&sys::gui_data, sizeof(sys::gui_data));
}

void sys::write_byte(std::string byte)
{
	if (byte == "")
	{
		printf("WRITE ERROR: No write data specified.\n");
		return;
	}

	// create payload vector and transmit data
	std::vector<uint8_t> tx_data = { (uint8_t)std::stoi(byte) };
	uart::send(uart::UART_FRAME_TYPE::RAW, tx_data);
}

void sys::write_array(std::string args)
// cmd: "write array 1 222 19 45" sends the bytes 1, 222, 19, 45
{
	if (args == "")
	{
		printf("WRITE ERROR: No write data specified.\n");
		return;
	}
	std::vector<std::string> str_array = cli::split_str(args);
	// parsed from args string
	for(int i = 0; i < str_array.size(); i++)
	{
		sys::write_byte(str_array[i]);
	}
}

void sys::write_spi(std::string args)
{
	// cmd: "write spi 1 17" sends the byte 17 to the 4-bit address 1
	// requires logic on the MCU
}

void sys::echo()
{
	// create payload vector and transmit data
	std::vector<uint8_t> tx_data = { 0x00 };
	uart::send(uart::UART_FRAME_TYPE::GET, tx_data);
}

void sys::set_mode(std::string args)
{
	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 1) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	uint8_t mode = 0;
	uint8_t uart_id = 0x00;

	// parse arguments
	// by number
	if (args.size() == 1)
	{
		mode = std::stoi(args);
	}
	// by string
	else
	{
		if (args == "idle")		mode = 0;
		if (args == "cal")		mode = 1;
		if (args == "tune")		mode = 2;
		if (args == "operate")	mode = 3;
	}

	// construct and send frame
	std::vector<uint8_t> tx_data = { uart_id, mode };
	uart::send(uart::UART_FRAME_TYPE::SET, tx_data);
}

void sys::set_gui(std::string args)
{
	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 1) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	uint8_t option = std::stoi(args);
	uint8_t uart_id = 0x03;

	// construct and send frame
	std::vector<uint8_t> tx_data = { uart_id, option };
	uart::send(uart::UART_FRAME_TYPE::SET, tx_data);
}

void sys::set_msg(std::string args)
{
	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 1) { return; }
	
	// construct variables to be correctly parsed by MCU & FPGA
	uint8_t option  = std::stoi(args);
	uint8_t uart_id = 0x04;

	// construct and send frame
	std::vector<uint8_t> tx_data = { uart_id, option };
	uart::send(uart::UART_FRAME_TYPE::SET, tx_data);
}

void sys::set_pwm(std::string args)
{
	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 2) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	// MOT0 = 0x01 & MOT1 = 0x02
	// e.g. set pwm 1 200 = set motor MOT1 (0x02) to pwm_val 200
	uint8_t motor	= std::stoi(args_vec[0]) + 1;
	uint8_t pwm_val	= std::stoi(args_vec[1]);
	uint8_t uart_id	= 0x01;

	// construct and send frame
	std::vector<uint8_t> tx_data = { uart_id, motor, pwm_val };
	uart::send(uart::UART_FRAME_TYPE::SET, tx_data);
}

void sys::set_freq(std::string args)
{
	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 2) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	// MOT0 = 0x01 & MOT1 = 0x02
	// e.g. set pwm 1 200 = set motor MOT1 (0x02) to pwm_val 200
	uint8_t motor = std::stoi(args_vec[0]) + 1;
	uint8_t freq_val = std::stoi(args_vec[1]);
	uint8_t uart_id = 0x02;

	// construct and send frame
	std::vector<uint8_t> tx_data = { uart_id, motor, freq_val };
	uart::send(uart::UART_FRAME_TYPE::SET, tx_data);
}

void sys::get_enc(std::string args)
{
	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 1) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	// ENC0 = 0x03 & ENC1 = 0x04
	// e.g. get enc 0 = read encoder ENC0 (0x03)
	uint8_t encoder	= std::stoi(args_vec[0]) + 3;
	uint8_t uart_id = 0x01;

	// construct and send frame
	std::vector<uint8_t> tx_data = { uart_id, encoder };
	uart::send(uart::UART_FRAME_TYPE::GET, tx_data);
}

void sys::get_hal(std::string args)
{
	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 1) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	// HAL0 = 0x05 & HAL1 = 0x06
	// e.g. get hal 0 = read hall sensor HAL0 (0x05)
	uint8_t hsen = std::stoi(args_vec[0]) + 5;
	uint8_t uart_id = 0x02;

	// construct and send frame
	std::vector<uint8_t> tx_data = { uart_id, hsen };
	uart::send(uart::UART_FRAME_TYPE::GET, tx_data);
}

void sys::shm::open()
{

	// important:
	// shared memoery name, buffer size etc. defined in "shm_def.h"

	sys::shm::h_map_file = CreateFileMapping(
		INVALID_HANDLE_VALUE,	// use paging file
		NULL,					// default security
		PAGE_READWRITE,			// read/write access
		0,						// maximum object size (high-order DWORD)
		buf_size,				// maximum object size (low-order DWORD)
		shm_name);				// name of mapping object

	if (sys::shm::h_map_file == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return;
	}

	sys::shm::data_buf = (LPTSTR)MapViewOfFile(
		sys::shm::h_map_file,		// handle to map object
		FILE_MAP_ALL_ACCESS,	// read/write permission
		0,
		0,
		buf_size);

	if (sys::shm::data_buf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(sys::shm::h_map_file);

		return;
	}
}

void sys::shm::close()
{
	UnmapViewOfFile(sys::shm::data_buf);
	CloseHandle(sys::shm::h_map_file);
}

void sys::shm::write(void* data, int size)
{
	// clear shared memory buffer
	ZeroMemory((PVOID)(sys::shm::data_buf), buf_size);

	// write desired data
	//CopyMemory((PVOID)(sys::shm::data_buf), &(sys::gui_data), sizeof(sys::gui_data));
	CopyMemory((PVOID)(sys::shm::data_buf), data, size);
}