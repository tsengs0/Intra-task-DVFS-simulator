#ifndef __PATTERN_GEN_H
#define __PATTERN_GEN_h
#include <iostream>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <vector>

#define default_freq 1000
#define LB_BB_loop 1
#define UB_BB_loop 5
#define LB_Branch_loop 1
#define UB_Branch_loop 5
#define LB_BB_num 5
#define UB_BB_num 20
enum {
	LB_1 = 1 ,
	UB_1 = 5 ,
	LB_2 = 6 ,
	UB_2 = 10,
	LB_3 = 11,
	UB_3 = 15,
	LB_4 = 16,
	UB_4 = 20
};

typedef struct Task_Info {
	float release_time;
	float start_time;
	int prt;
	float rel_dline;
	float wcet; // unit: us
	float period;
	bool NRT_USED; // A flag used to label whether the polling of release_time have been detected/used or not
	char state;
	float wcrt;
//	Task_Info(int r, int pr, int rd, int wc, int p, char st)
//	: release_time(r), prt(pr), rel_dline(rd), wcet(wc), period(p), state(st) {}
} task_info_t;

typedef struct TestBench_Param {
	int N_branch;
	int N_loop;
	int N_BB_loop;
	int N_Branch_loop;
} testbench_param;

typedef struct BB_t {
	int block_index;
	int execution_cycle_w;
	int execution_cycle_a;
	int execution_cycle_b;
	std::vector<int> succ;
	BB_t(int a, int b, int c, int d, std::vector<int> e)
	: block_index(a), execution_cycle_w(b), execution_cycle_a(c), execution_cycle(d), succ(e) {}
} BB;
class testbench {
	private:
		int tasks_num;
		task_info_t *tasks;
		int *N_init;
		int *BB_min, *BB_max,*BB_avg;
		int *wcec;
		testbench_param *TaskCfg_param;
		std::vector<BB> ExePath_case; 

	public:
		testbench(int TskNum, task_info_t *task_set);
		~testbench();
		void CfgParam_config(
			int TskNum,                  // The number of tasks
			unsigned char *branch_level, // The level of branch numbers, ranging from [LB_1,UB_1] to [LB_4,UB_4]
			unsigned char *loop_level    // The level of loop numbers, ranging from [LB_1,UB_1] to [LB_4,UB_4]
		);
		void cfg_gen(int TskID);
		void param_show();
};

#endif // __PATTERN_GEN_H
