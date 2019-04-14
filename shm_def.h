#pragma once

// constructs
struct MOT_DATA
{
	int		pwm;
	int		freq;
	int		enc;
	float	spd;
	int		hal;

	int		pid_i;
	int		pid_n;
	float	pid_kp;
	float	pid_ki;
	float	pid_kd;
};

struct GUI_DATA
{
	int mode;

	MOT_DATA	mot0;
	MOT_DATA	mot1;
};

// constants
constexpr auto shm_name = "shm_pan_tilt_gui";
constexpr auto buf_size = sizeof(GUI_DATA);
