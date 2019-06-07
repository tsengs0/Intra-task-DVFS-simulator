#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "../inc/timer.h"

using std::cout;
using std::endl;
using std::vector;

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
double Time_Management::time_unit_config(double time_val)
{
	return time_val / sys_clk -> time_unit;
}

/*
	Updating the "sys_clk -> cur_time" by addition of acc_time
*/
void Time_Management::update_cur_time(double acc_time)
{
	sys_clk -> cur_time = acc_time;
}

/*
	Configuring the current speed
*/
void Time_Management::cur_freq_config(double freq_in)
{
	sys_clk -> cur_freq = freq_in;
#ifdef DEBUG	
	cout << "Configure current operating frequency at " << sys_clk -> cur_freq << " MHz" << endl;
#endif
}

double Time_Management::show_cur_freq(void)
{
	return sys_clk -> cur_freq;
}

/**
 *  @brief Update/re-calculate the execution time which current instance of currently running task has been using so far from its start time
**/
void Time_Management::ExecutedTime_Accumulator(unsigned char curCase, int running_task_id)
{
	/*==================================================================*/
	// Update point of exeucuted time:
	// (Case 1) start time
	// (Case 2) frequency scaling point
	// (Case 3) preempted point
	// (Case 4) resume point
	// (Case 5) completion time
	// (Case 6) power evaluation point (done by system)
	/*==================================================================*/
	switch(curCase) {
		case (unsigned char) START_TIME:
					ExecutedTime[running_task_id] = (double) 0.0;
					UpdatePoint = (double) (sys_clk -> cur_time);
					break;
		case (unsigned char) FREQ_SCALING_POINT:
					ExecutedTime[running_task_id] += (double) ((sys_clk -> cur_time) - UpdatePoint);
					UpdatePoint = (double) (sys_clk -> cur_time);
					break;
		case (unsigned char) PREEMPTED_POINT:
					ExecutedTime[running_task_id] += (double) ((sys_clk -> cur_time) - UpdatePoint);
					break;
		case (unsigned char) RESUME_POINT:
					UpdatePoint = (double) (sys_clk -> cur_time);
					break;
		case (unsigned char) COMPLETION_TIME:
					ExecutedTime[running_task_id] += (double) ((sys_clk -> cur_time) - UpdatePoint);
					break;
		case (unsigned char) SYS_POWER_EVAL_POINT:
					ExecutedTime[running_task_id] += (double) ((sys_clk -> cur_time) - UpdatePoint);
					UpdatePoint = (double) (sys_clk -> cur_time);
					break;
		default:
					ExecutedTime[running_task_id] = ExecutedTime[running_task_id];
					UpdatePoint = UpdatePoint;
					break;
	}
}
