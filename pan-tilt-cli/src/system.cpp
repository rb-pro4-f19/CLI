#include "system.h"

//// Private Declarations /////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace sys
{

	enum STREAM_FRAME_SIZE
	{
		STREAM_DAT_FRAME = 1,
		STREAM_CMD_FRAME = 2
	};

	struct SAMPLE_DATAPOINT
	{
		uint16_t	time;
		int16_t		value;
	};

	GUI_DATA gui_data;

	DWORD	gPidToFind = 0;
	HWND	gTargetWindowHwnd = NULL;
	BOOL CALLBACK myWNDENUMPROC(HWND hwCurHwnd, LPARAM lpMylp);

	std::function<void(std::string filename)> callback_sample_data;

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

template <typename... T >
std::string sstr(T &&... args)
{
	std::ostringstream sstr;
	// fold expression
	((sstr << std::dec) << ... << args);
	return sstr.str();
}

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

	// SAMPLE_DATA callback
	uart::reciever::callback_sam = sys::sample_data_handler;

	// connect UART to specified or default COM port
	(com_port == "") ? uart::connect() : uart::connect(com_port.c_str());

	// sleep a while
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	// disable msg
	;
}

void sys::echo()
{
	// create payload vector and transmit data
	CMD_ID cmd_id = DO_ECHO;
	std::vector<uint8_t> tx_data = { (uint8_t)cmd_id };
	uart::send(uart::UART_FRAME_TYPE::UART_DO, tx_data);
}

void sys::gui()
{
	// check if allready open
	//if (sys::gui_open) { return; }

	// init GUI data
	sys::gui_data.mode = 0;
	//sys::gui_data.mot0 = {  12, 80, 0, 0.0f, false, 0, 0, 10.0f, 0.0f, 0.0f };
	//sys::gui_data.mot1 = { 113, 80, 0, 0.0f, false, 0, 0, 20.0f, 0.0f, 0.0f };

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

void sys::sample_new(std::string args)
{
	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() < 3) { return; }

	// variables for MCU
	// structs copied from MCU C code

	enum SYSTEM_VAR
	{
		SV_ADDR,
		SV_PID0_U,
		SV_PID0_Y,
		SV_PID1_U,
		SV_PID1_Y,
	} target_var;

	enum SAMPLE_TYPE
	{
		ST_INTEGER,
		ST_FLOATING,
	} target_type;

	static uint16_t	target_samples = 0;
	static uint32_t target_addr = 0;
	static uint8_t	target_samples_bytearr[sizeof(uint16_t)];
	static uint8_t	target_addr_bytearr[sizeof(uint32_t)];

	if (args_vec[0].substr(0, 2) == "0x")
	// "sample new 0x<addr> <type> <dur>"
	{
		// set target variable
		target_var = SV_ADDR;

		// parse hex address from string
		target_addr = std::stoi(args_vec[0].substr(2), 0, 16);

		// convert uint32_t into byte array
		memcpy(target_addr_bytearr, &target_addr, sizeof(uint32_t));
	}
	else
	// "sample new <var> <type> <dur>"
	{
		if (args_vec[0] == "pid0_u")	{ target_var = SV_PID0_U; }
		if (args_vec[0] == "pid0_y")	{ target_var = SV_PID0_Y; }
		if (args_vec[0] == "pid1_u")	{ target_var = SV_PID1_U; }
		if (args_vec[0] == "pid1_y")	{ target_var = SV_PID1_Y; }
	}

	// parse target type
	if (args_vec[1] == "int")	{ target_type = ST_INTEGER; }
	if (args_vec[1] == "float")	{ target_type = ST_FLOATING; }

	// parse target duration & convert into byte arrray
	target_samples = std::stoi(args_vec[2]);
	memcpy(target_samples_bytearr, &target_samples, sizeof(uint16_t));

	// construct variables to be correctly parsed by MCU & FPGA
	CMD_ID cmd_id = DO_SAMPLE;

	// transmission vector
	std::vector<uint8_t> tx_data = { (uint8_t)cmd_id, (uint8_t)target_var, (uint8_t)target_type};

	// insert target_samples (uint16_t) byte array into tx_data vector
	tx_data.insert(tx_data.end(), &target_samples_bytearr[0], &target_samples_bytearr[sizeof(uint16_t)]);

	// insert target_addr (uint32_t) byte array into tx_data vector
	tx_data.insert(tx_data.end(), &target_addr_bytearr[0], &target_addr_bytearr[sizeof(uint32_t)]);

	// send data
	uart::send(uart::UART_FRAME_TYPE::UART_DO, tx_data);
}

void sys::step(std::string args)
{
	// step response command
	// "step pid<pid>_<var> <dur_ms>" e.g. "step pid0_u 500"
	// "step pid<pid>_<var> <dur_ms> plot" to enable plotting afterwards e.g. "step pid0_u 500 plot"

	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);
	auto pid_id = args_vec[0].substr(3, 1);

	// check that correct num of parameters was passed
	if (args_vec.size() < 2) { return; }

	// reset/home position
	sys::set_pos("0 0");

	// sleep a while
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	// sample arguments
	auto sample_args = sstr(args_vec[0], " ", "float", " ", args_vec[1]);

	// begin sampling
	sys::sample_new(sample_args);

	// set step position according to pid_id
	// 0 = tilt & 1 = pan
	if (pid_id == "0") { sys::set_pos("0 90"); }
	if (pid_id == "1") { sys::set_pos("45 0"); }

	// plot data if necessary
	if (args_vec.size() == 3 && args_vec[2] == "plot")
	{
		sys::callback_sample_data = [&](std::string filename)
		{
			// run MATLAB script/function
			auto cmd = sstr("matlab -nodesktop -r \"plot_step('", filename, "')\"");
			system(cmd.c_str());

			// reset plot callback
			sys::callback_sample_data == nullptr;
		};
	}
}

void sys::sample_resend(std::string args)
{
	// create payload vector and transmit data
	CMD_ID cmd_id = DO_RESEND;
	std::vector<uint8_t> tx_data = { (uint8_t)cmd_id };
	uart::send(uart::UART_FRAME_TYPE::UART_DO, tx_data);
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

	// flush if queue too large (outdated)
	if (uart::buffer::queue_size() > UART_BUFFER_FLUSH_TH) { uart::buffer::flush(); }
}

void sys::sample_data_handler(uart::UART_FRAME frame)
{
	static SYSTEMTIME			lt;
	static CString				timestamp;

	enum UART_SAMPLEDATA_CMD
	{
		USDC_RESET,
		USDC_DATAPOINT,
		USDC_EOT,
	};

	static SAMPLE_DATAPOINT rx_data;

	constexpr auto max_num_bytes = sizeof(rx_data);

	static UART_SAMPLEDATA_CMD rx_state;
	static uint8_t*	rx_data_ptr;
	static int		rx_num_bytes;
	static int		rx_num_total_bytes;

	static std::vector<SAMPLE_DATAPOINT> sample_data;

	rx_num_total_bytes++;

	if (frame.size == 2)
	{
		rx_state = (UART_SAMPLEDATA_CMD)frame.payload[0];

		switch (rx_state)
		{
			case USDC_RESET:
			{
				rx_data_ptr = nullptr;
				sample_data.clear();
				sample_data.reserve(8192);
				rx_num_total_bytes = 0;

				break;
			}

			case USDC_DATAPOINT:
			{
				// write previous data out
				if (rx_data_ptr != nullptr)
				{
					sample_data.push_back(rx_data);
				}

				// reset rx_data
				memset(&rx_data, 0, sizeof(rx_data));

				// set pointer
				rx_data_ptr = (uint8_t*)(&rx_data);

				// reset counter
				rx_num_bytes = 0;
				break;
			}

			case USDC_EOT:
			{
				// check if there is data to output
				if (sample_data.empty()) { break; }

				// get system time
				GetLocalTime(&lt);

				// format timestamp
				timestamp.Format
				(
					"%02d%02d_%02d%02d%02d",
					lt.wMonth,
					lt.wDay,
					lt.wHour,
					lt.wMinute,
					lt.wSecond
				);

				// format filename
				CreateDirectory("sample_data/", NULL);
				std::string filename = "sample_data/samples_" + std::string(timestamp) + ".dat";
				std::ofstream out_fstream(filename);

				// output data to file
				for (const auto& v : sample_data)
				{
					out_fstream << v.time << ";" << std::fixed << (float)v.value << "\n";
				}

				// close filestream
				out_fstream.close();

				// print info
				printf("\nFile \"%s\" was recieved; total of %d bytes.\n", filename.c_str(), rx_num_total_bytes);

				// invoke callback if specified
				if (sys::callback_sample_data != nullptr) { sys::callback_sample_data(filename); }

				// reset everything in case reset is never called
				rx_data_ptr = nullptr;
				sample_data.clear();

				break;
			}
		}
	}
	else if (rx_state == USDC_DATAPOINT && rx_num_bytes < max_num_bytes && rx_data_ptr != nullptr)
	{
		// append data to struct
		*rx_data_ptr++ = frame.payload[0];
		rx_num_bytes++;
	}
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
	//uart::send(uart::UART_FRAME_TYPE::UART_RAW, tx_data);
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

void sys::set_mode(std::string args)
{
	// set mode <mode>

	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 1) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	uint8_t mode = 0;
	CMD_ID cmd_id = SET_MODE;

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
	std::vector<uint8_t> tx_data = { (uint8_t)cmd_id, mode };
	uart::send(uart::UART_FRAME_TYPE::UART_SET, tx_data);
}

void sys::set_pos(std::string args)
{
	// set the position given in degrees
	// set pos <pan> <tilt>

	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() == 1) { return; }

	// parse desired angles
	float theta_pan		= std::stof(args_vec[0]);
	float theta_tilt	= std::stof(args_vec[1]);

	// guard pan angle
	//if (abs(theta_pan) > 80)

	// motor id's (SPI ADDRESS)
	uint8_t mot0 = 0x01;
	uint8_t mot1 = 0x02;

	// set position
	sys::set_pos_single(mot1, theta_pan);
	sys::set_pos_single(mot0, theta_tilt);
}

void sys::set_pos_single(uint8_t mot_id, float value)
{
	// construct variables to be correctly parsed by MCU & FPGA
	CMD_ID cmd_id = SET_POS;

	// convert float into byte array
	unsigned char flt_array[sizeof(float)];
	memcpy(flt_array, &value, sizeof(float));

	// construct tx_data vector
	std::vector<uint8_t> tx_data = { (uint8_t)cmd_id, mot_id };

	// insert float byte array into tx_data vector
	tx_data.insert(tx_data.end(), &flt_array[0], &flt_array[sizeof(float)]);

	// send tx_data
	uart::send(uart::UART_FRAME_TYPE::UART_SET, tx_data);
}

void sys::set_gui(std::string args)
{
	// set gui <bool>

	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 1) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	uint8_t option = std::stoi(args);
	CMD_ID cmd_id = SET_GUI;

	// construct and send frame
	std::vector<uint8_t> tx_data = { (uint8_t)cmd_id, option };
	uart::send(uart::UART_FRAME_TYPE::UART_SET, tx_data);
}

void sys::set_msg(std::string args)
{
	// set msg <bool>

	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 1) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	uint8_t option  = std::stoi(args);
	CMD_ID cmd_id = SET_MSG;

	// construct and send frame
	std::vector<uint8_t> tx_data = { (uint8_t)cmd_id, option };
	uart::send(uart::UART_FRAME_TYPE::UART_SET, tx_data);
}

void sys::set_pwm(std::string args)
{
	// set pwm <id> <pwm>

	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 2) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	// MOT0 = 0x01 & MOT1 = 0x02
	// e.g. set pwm 1 200 = set motor MOT1 (0x02) to pwm_val 200
	uint8_t mot_id	= std::stoi(args_vec[0]) + 1;
	uint8_t pwm_val	= std::stoi(args_vec[1]);
	CMD_ID cmd_id = SET_PWM;

	// construct and send frame
	std::vector<uint8_t> tx_data = { (uint8_t)cmd_id, mot_id, pwm_val };
	uart::send(uart::UART_FRAME_TYPE::UART_SET, tx_data);
}

void sys::set_freq(std::string args)
{
	// set freq <id> <freq>

	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 2) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	// MOT0 = 0x01 & MOT1 = 0x02
	// e.g. set pwm 1 200 = set motor MOT1 (0x02) to pwm_val 200
	uint8_t mot_id = std::stoi(args_vec[0]) + 1;
	uint8_t freq_val = std::stoi(args_vec[1]);
	CMD_ID cmd_id = SET_FREQ;

	// construct and send frame
	std::vector<uint8_t> tx_data = { (uint8_t)cmd_id, mot_id, freq_val };
	uart::send(uart::UART_FRAME_TYPE::UART_SET, tx_data);
}

void sys::set_pid(std::string args)
{
	// set pid <id> <param> <val>
	// set pid <id> all <kp> <ki> <kd>

	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() < 2) { return; }

	// want to set all parameters?
	bool set_all = (args_vec[1] == "all");

	// check that correct num of parameters was passed if trying to set all
	if (set_all && args_vec.size() < 5) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	// PID0/MOT0 = 0x01 & PID1/MOT1 = 0x02
	// e.g. set pid 1 ... = set PID that controls MOT1 (0x02) to ...
	uint8_t pid_id = std::stoi(args_vec[0]) + 1;

	// check which pid parameters to set
	if (args_vec[0] == "all")
	// set pid <id> all <kp> <ki> <kd>
	{
		sys::set_pid_param(pid_id, PID_PARAM::PID_KP, std::stof(args_vec[2]));
		sys::set_pid_param(pid_id, PID_PARAM::PID_KI, std::stof(args_vec[3]));
		sys::set_pid_param(pid_id, PID_PARAM::PID_KD, std::stof(args_vec[4]));
	}
	// set pid <id> <param> <val>
	else if (args_vec[1] == "kp")		{ sys::set_pid_param(pid_id, PID_PARAM::PID_KP,		std::stof(args_vec[2])); }
	else if (args_vec[1] == "ki")		{ sys::set_pid_param(pid_id, PID_PARAM::PID_KI,		std::stof(args_vec[2])); }
	else if (args_vec[1] == "kd")		{ sys::set_pid_param(pid_id, PID_PARAM::PID_KD,		std::stof(args_vec[2])); }
	else if (args_vec[1] == "n")		{ sys::set_pid_param(pid_id, PID_PARAM::PID_N,		std::stof(args_vec[2])); }
	else if (args_vec[1] == "clamp")	{ sys::set_pid_param(pid_id, PID_PARAM::PID_CLAMP,	std::stof(args_vec[2])); }
}

void sys::set_pid_param(uint8_t pid_id, PID_PARAM pid_param, float value)
{
	// construct variables to be correctly parsed by MCU & FPGA
	CMD_ID cmd_id = SET_PID;

	// convert float into byte array
	//char const* flt_array = reinterpret_cast<char const *>(&value);
	unsigned char flt_array[sizeof(float)];
	memcpy(flt_array, &value, sizeof(float));

	// construct tx_data vector
	std::vector<uint8_t> tx_data = { (uint8_t)cmd_id, (uint8_t)pid_id, (uint8_t)pid_param };

	// insert float byte array into tx_data vector
	tx_data.insert(tx_data.end(), &flt_array[0], &flt_array[sizeof(float)]);

	// send tx_data
	uart::send(uart::UART_FRAME_TYPE::UART_SET, tx_data);

	// code to recieve data
	//float f = 0.0;
	//memcpy(&f, flt_array, sizeof(float))
	//printf("Value: %f.\n", f);
}

void sys::set_slew(std::string args)
{
	// set slew <id> <bool> e.g. "set slew r 0"

	// construcs
	enum TARGET_SLEW
	{
		TARGET_SLEW_REF,
		TARGET_SLEW_MOT
	};

	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 2) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	TARGET_SLEW target_slew;
	bool option;
	CMD_ID cmd_id = SET_SLEW;

	// parse target slew
	if (args_vec[0] == "r") { target_slew = TARGET_SLEW_REF; }
	if (args_vec[0] == "y") { target_slew = TARGET_SLEW_MOT; }

	// parse option
	option = std::stoi(args_vec[1]);

	// construct and send frame
	std::vector<uint8_t> tx_data = { (uint8_t)cmd_id, (uint8_t)target_slew, (uint8_t)option };
	uart::send(uart::UART_FRAME_TYPE::UART_SET, tx_data);
}

void sys::set_bound(std::string args)
{
	// set bound <bool>

	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 1) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	uint8_t option = std::stoi(args);
	CMD_ID cmd_id = SET_BOUND;

	// construct and send frame
	std::vector<uint8_t> tx_data = { (uint8_t)cmd_id, option };
	uart::send(uart::UART_FRAME_TYPE::UART_SET, tx_data);
}

void sys::get_enc(std::string args)
{
	// get enc <id>

	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 1) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	// ENC0 = 0x03 & ENC1 = 0x04
	// e.g. get enc 0 = read encoder ENC0 (0x03)
	uint8_t enc_id	= std::stoi(args_vec[0]) + 3;
	CMD_ID cmd_id = GET_ENC;

	// construct and send frame
	std::vector<uint8_t> tx_data = { (uint8_t)cmd_id, enc_id };
	uart::send(uart::UART_FRAME_TYPE::UART_GET, tx_data);
}

void sys::get_hal(std::string args)
{
	// get hal <id>

	// split input delimited by spaces into vector of strings
	auto args_vec = cli::split_str(args);

	// check that correct num of parameters was passed
	if (args_vec.size() != 1) { return; }

	// construct variables to be correctly parsed by MCU & FPGA
	// HAL0 = 0x05 & HAL1 = 0x06
	// e.g. get hal 0 = read hall sensor HAL0 (0x05)
	uint8_t hsen_id = std::stoi(args_vec[0]) + 5;
	CMD_ID cmd_id = GET_HAL;

	// construct and send frame
	std::vector<uint8_t> tx_data = { (uint8_t)cmd_id, hsen_id };
	uart::send(uart::UART_FRAME_TYPE::UART_GET, tx_data);
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
