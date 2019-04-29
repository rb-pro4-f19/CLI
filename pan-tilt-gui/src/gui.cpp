#include "gui.h"

//// GUI Defines //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define BORDER		cout << "###" << setfill('#') << setw(FULLWIDTH) << "" << "###" << setfill(' ') << endl;
#define	BORDERL		"#  "
#define	BORDERR		"  #"
#define COLSEP		"  |  "
#define COLSEP2		"     "

#define COL1		16
#define COL2		16
#define COLPID		8
#define COLSEPW		5
#define FULLWIDTH	(COL1 + COL2) * 2 + COLSEPW

#define ROW(x)		cout << BORDERL << x << BORDERR << endl;
#define LN(x)		cout << BORDERL << setw(FULLWIDTH) << left << x << BORDERR << endl;
#define EMPTYLN		cout << BORDERL << setfill(' ') << setw(FULLWIDTH) << "" << BORDERR << setfill(' ') << endl;
#define COLSEPLN	cout << BORDERL << setfill(' ') << setw(COL1 + COL2) << left << "" << setw(COLSEPW) << COLSEP << setw(COL1 + COL2) << left << "" << BORDERR << setfill(' ') << endl;
#define HORLN		cout << BORDERL << setfill('-') << setw(COL1 + COL2) << left << "" << setw(COLSEPW) << COLSEP << setw(COL1 + COL2) << left << "" << BORDERR << setfill(' ') << endl;

//// Private Declarations /////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace gui
{
	GUI_DATA	data;
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

void gui::init()
{
	// initial clear
	system("cls");

	gui::hidecur();

	// init data
	gui::data.mode = 0;
	//gui::data.mot0 = { 0, 0, 0, 0.0f, false, 0, 0, 0.0f, 0.0f, 0.0f };
	//gui::data.mot1 = { 0, 0, 0, 0.0f, false, 0, 0, 0.0f, 0.0f, 0.0f };
}

void gui::hidecur()
{
	// hide cursor
	std::cout << "\033[?25l";
}

void gui::read_shm()
{
	static HANDLE		h_map_file;
	static GUI_DATA*	data_buf;

	h_map_file = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		shm_name);             // name of mapping object

	if (h_map_file == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		return;
	}

	data_buf = (GUI_DATA*)MapViewOfFile(h_map_file, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		buf_size);

	if (data_buf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(h_map_file);

		return;
	}

	// copy data
	gui::data = *data_buf;

	UnmapViewOfFile(data_buf);
	CloseHandle(h_map_file);
}

void gui::draw()
{
	using namespace std;

	// move cursor to 00
	std::cout << "\033[00H";

	// print data

	// top border
	BORDER
	EMPTYLN

	LN("Pan-Tilt System GUI")
	EMPTYLN
	
	static std::string mode_str = "";

	switch ((int)gui::data.mode)
	{
		case 0:		mode_str = "idle"; break;
		case 1:		mode_str = "calibration"; break;
		case 2:		mode_str = "tuning"; break;
		case 3:		mode_str = "operating"; break;
		default:	mode_str = "undefined";
	}

	//LN(sstr("Mode: ", (int)gui::data.mode, " (", mode_str, ")"));

	cout << BORDERL <<
		setw(COL1) << left << "Mode:" << setw(COL2) << right << sstr(mode_str, " (", (int)gui::data.mode, ")") <<
		setw(COLSEPW) << COLSEP2 <<
		setw(COL1) << left << "Operation time:" << setw(COL2) << right << sstr((int)gui::data.op_time, " ms") <<
	BORDERR << endl;


	cout << BORDERL <<
		setw(COL1) << left << "Calibrated:" << setw(COL2) << right << (gui::data.cal_done ? "true" : "false") <<
		setw(COLSEPW) << COLSEP2 <<
		setw(COL1) << left << "Boundary:" << setw(COL2) << right << (gui::data.bound ? "enabled" : "disabled") <<
	BORDERR << endl;

	cout << BORDERL <<
		setw(COL1) << left << "Slew (r):" << setw(COL2) << right << (gui::data.slew_r ? "enabled" : "disabled") <<
		setw(COLSEPW) << COLSEP2 <<
		setw(COL1) << left << "Slew (y):" << setw(COL2) << right << (gui::data.slew_y ? "enabled" : "disabled") <<
	BORDERR << endl;

	EMPTYLN
	BORDER
	EMPTYLN

	// ...
	cout << BORDERL <<
		setw(COL1) << left << "MOTOR0:" << setw(COL2) << right << "" <<
		setw(COLSEPW) << COLSEP <<
		setw(COL1) << left << "MOTOR1:" << setw(COL2) << right << "" <<
	BORDERR << endl;

	// empty line w/ # in middle
	COLSEPLN

	// horizontal line
	HORLN

	// empty line w/ # in middle
	COLSEPLN

	// PWM
	cout << BORDERL <<
		setw(COL1) << left << "PWM:" << setw(COL2) << right << (int)gui::data.mot0.pwm <<
		setw(COLSEPW) << COLSEP <<
		setw(COL1) << left << "PWM:" << setw(COL2) << right << (int)gui::data.mot1.pwm <<
	BORDERR << endl;

	// Frequency
	cout << BORDERL <<
		setw(COL1) << left << "Frequency:" << setw(COL2) << right << (int)gui::data.mot0.freq <<
		setw(COLSEPW) << COLSEP <<
		setw(COL1) << left << "Frequency:" << setw(COL2) << right << (int)gui::data.mot1.freq <<
	BORDERR << endl;

	// Speed
	/*cout << BORDERL <<
		setw(COL1) << left << "Speed:" << setw(COL2) << right << (float)gui::data.mot0.spd <<
		setw(COLSEPW) << COLSEP <<
		setw(COL1) << left << "Speed:" << setw(COL2) << right << (float)gui::data.mot1.spd <<
	BORDERR << endl;*/

	// Encoder
	cout << BORDERL <<
		setw(COL1) << left << "Encoder:" << setw(COL2) << right << (int)gui::data.mot0.enc <<
		setw(COLSEPW) << COLSEP <<
		setw(COL1) << left << "Encoder:" << setw(COL2) << right << (int)gui::data.mot1.enc <<
	BORDERR << endl;

	// Hall Sensor
	cout << BORDERL <<
		setw(COL1) << left << "Hall:" << setw(COL2) << right << (int)gui::data.mot0.hal <<
		setw(COLSEPW) << COLSEP <<
		setw(COL1) << left << "Hall:" << setw(COL2) << right << (int)gui::data.mot1.hal <<
	BORDERR << endl;

	// empty line w/ # in middle
	COLSEPLN

	// PID / i
	cout << BORDERL <<
		setw(COL1) << left << "PID:" << setw(COLPID) << left << "i: " << setw(COL2 - COLPID) << right << sstr(2, "/", 5) <<
		setw(COLSEPW) << COLSEP <<
		setw(COL1) << left << "PID:" << setw(COLPID) << left << "i: " << setw(COL2 - COLPID) << right << sstr(1, "/", 1) <<
	BORDERR << endl;

	// PID / Kp
	cout << BORDERL <<
		setw(COL1) << left << "" << setw(COLPID) << left << "Kp: " << setw(COL2 - COLPID) << right << (float)gui::data.mot0.pid_kp <<
		setw(COLSEPW) << COLSEP <<
		setw(COL1) << left << "" << setw(COLPID) << left << "Kp: " << setw(COL2 - COLPID) << right << (float)gui::data.mot1.pid_kp <<
	BORDERR << endl;

	// PID / Ki
	cout << BORDERL <<
		setw(COL1) << left << "" << setw(COLPID) << left << "Ki: " << setw(COL2 - COLPID) << right << (float)gui::data.mot0.pid_ki <<
		setw(COLSEPW) << COLSEP <<
		setw(COL1) << left << "" << setw(COLPID) << left << "Ki: " << setw(COL2 - COLPID) << right << (float)gui::data.mot1.pid_ki <<
	BORDERR << endl;

	// PID / Kd
	/*cout << BORDERL <<
		setw(COL1) << left << "" << setw(COLPID) << left << "Kd: " << setw(COL2 - COLPID) << right << (float)gui::data.mot0.pid_kd <<
		setw(COLSEPW) << COLSEP <<
		setw(COL1) << left << "" << setw(COLPID) << left << "Kd: " << setw(COL2 - COLPID) << right << (float)gui::data.mot1.pid_kd <<
	BORDERR << endl;*/

	// PID / Clamp
	cout << BORDERL <<
		setw(COL1) << left << "" << setw(COLPID) << left << "Clamp: " << setw(COL2 - COLPID) << right << (float)gui::data.mot0.pid_clamp <<
		setw(COLSEPW) << COLSEP <<
		setw(COL1) << left << "" << setw(COLPID) << left << "Clamp: " << setw(COL2 - COLPID) << right << (float)gui::data.mot1.pid_clamp <<
	BORDERR << endl;

	// PID / n
	/*cout << BORDERL <<
		setw(COL1) << left << "" << setw(COLPID) << left << "n: " << setw(COL2 - COLPID) << right << (int)gui::data.mot0.pid_n <<
		setw(COLSEPW) << COLSEP <<
		setw(COL1) << left << "" << setw(COLPID) << left << "n: " << setw(COL2 - COLPID) << right << (int)gui::data.mot1.pid_n <<
	BORDERR << endl;*/

	// bottom border
	EMPTYLN
	BORDER
}