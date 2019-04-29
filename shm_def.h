#pragma once

// !!!
// CONSTRUCTS MUST ALSO BE UPDATED ON THE MCU
// OTHERWISE DATA TRANSMISSION WILL FAIL.

// constructs
struct MOT_DATA
{
	int8_t		pwm;
	uint8_t		freq;
	int16_t		enc;
	//float		spd;
	uint8_t		hal;

	//uint8_t		pid_i;
	//uint8_t		pid_n;
	float		pid_kp;
	float		pid_ki;
	//float		pid_kd;
	uint8_t		pid_clamp;
};

struct GUI_DATA
{
	uint8_t		mode;
	uint16_t 	op_time;
	uint8_t 	cal_done;
	uint8_t		bound;
	uint8_t		slew_r;
	uint8_t		slew_y;

	MOT_DATA	mot0;
	MOT_DATA	mot1;
};

// constants
constexpr auto shm_name = "shm_pan_tilt_gui";
constexpr auto buf_size = sizeof(GUI_DATA);
