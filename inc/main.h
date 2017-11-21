#ifndef __MAIN_H
#define __MAIN_H

#define DEBUG
//#define RT_SIMULATOR
#define INTRA_SCHED
#define US 1
#define SEC 1000000
#define S_MS 1000
#define INST_UNIT 1 // Each instruction takes 1 execution cycles (=1 CPI)
#define N_DECIMAL_POINTS_PRECISION 1000.0 // To extracting three decimal points

#define tasks_num 3
#define patterns_num 3

/*
 cur_freq (MHz)
 cur_time (us)
*/
typedef struct Sys_Clk {
	float cur_freq; // The current execution speed for the system
	float cur_time;  // The current system tick (elapsed time so far) 
	int time_unit; // Unit of time (ms)
} sys_clk_t;

enum {
	WORST   = 1, // The worst case
	AVERAGE = 2, // The average case
	BEST    = 3  // The best case
};

enum {
	ATTRIBUTES_ITEM = 0x01, // "{}" for task attributes
	TASK_ATTRIBUTES = 0x02, // For contents of task attributes
	SUCCESSORS_ITEM = 0x03, // "{}" for its successors
	SUCCESSORS	= 0x04  // For contents of its successors
};

enum {
	TASK_ID = 0x05,
	WCEC    = 0x06,
	ACEC    = 0x07,
	BCEC    = 0x08
};

enum {
	LOOP      = 0x01, // Belonging to a certain loop nest
	NON_LOOP  = 0x02, // Not inside any loop nest
	LOOP_EXIT = 0x03  // The exit point of a certain loop nest
};

enum {
	NOT_TAKEN     = 1, // If branch instruction was not taken
	TAKEN         = 3  // If branch instruction was taken
};

enum {
	DVFS_ENABLE  = 0x01, // Enabling the DVFS
	DVFS_DISABLE = 0x02  // Disabling the DVFS
};

enum {
	H_RESP  = 1, // High Responsiveness
	L_POWER = 2, // Low Power 
};

enum {
	START_TIME      = 0x01,
	PREEMPTED_POINT = 0x02,
	RESUME_POINT    = 0x03,
	COMPLETION_TIME = 0x04
};

typedef int exeTime_info[3];

#endif // __MAIN_H

