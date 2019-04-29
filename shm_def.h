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
	float		spd;
	uint8_t		hal;

	uint8_t		pid_i;
	uint8_t		pid_n;
	float		pid_kp;
	float		pid_ki;
	float		pid_kd;
};

struct GUI_DATA
{
	uint8_t		mode;

	MOT_DATA	mot0;
	MOT_DATA	mot1;
};

// constants
constexpr auto shm_name = "shm_pan_tilt_gui";
constexpr auto buf_size = sizeof(GUI_DATA);
