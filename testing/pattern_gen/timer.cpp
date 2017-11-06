#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "timer.h"

using namespace std;

Time_Management::Time_Management(sys_clk_t &clk)
{
	// Designating the system-clock/timer source
	sys_clk = &clk;
}

Time_Management::~Time_Management(void)
{

}

/*
	Changing the time-unit of time_val basing on the "sys_clk -> time_unit"
*/
float Time_Management::time_unit_config(float time_val)
{
	return time_val / sys_clk -> time_unit;
}

/*
	Updating the "sys_clk -> cur_time" by addition of acc_time
*/
void Time_Management::update_cur_time(float acc_time)
{
	sys_clk -> cur_time = acc_time;
}

/*
	Configuring the current speed
*/
void Time_Management::cur_freq_config(float freq_in)
{
	sys_clk -> cur_freq = freq_in;	
}
