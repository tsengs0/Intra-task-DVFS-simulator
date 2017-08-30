#ifndef __RT_SIMULATOR_H
#define __RT_SIMULATOR_H

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "main.h"
#include "dvfs_info.h"
#include "timer.h"
#include "sched.h"
#include "inter_bus.h"
#include "cfg_info.h"

using namespace std;

class Task_Management : public Task_Scheduler, public Src_CFG {
	private:

	public:
		 
};

#endif // __RT_SIMULATOR
