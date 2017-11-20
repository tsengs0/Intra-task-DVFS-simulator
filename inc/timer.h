#ifndef __TIMER_H
#define __TIMER_H

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "main.h"

using namespace std;

class Time_Management {
	private:
	public:
		sys_clk_t *sys_clk;
		float *ExecutedTime; // From Start time until now, how many execution time have been used by current instance of current task
		float UpdatePoint; // Fo update the current time in order to calculate the length of executed time by currently running task's current instance
		Time_Management(sys_clk_t &clk);
		~Time_Management(void);

		float time_unit_config(float time_val);
		void update_cur_time(float acc_time);
		void cur_freq_config(float freq_in);
		void ExecutedTime_Accumulator(unsigned char curCase, int running_task_id);
};

#endif // __TIMER_H
