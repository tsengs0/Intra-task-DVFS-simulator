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
		Time_Management(sys_clk_t &clk);
		~Time_Management(void);

		float time_unit_config(float time_val);
		void update_cur_time(float acc_time);
		void cur_freq_config(float freq_in);
};

#endif // __TIMER_H
