#ifndef __PATTERN_GEN_H
#define __PATTERN_GEN_h
#include <iostream>
#include <ctime>
#include <cstdio>
#include <cstdlib>

#define LB_BB_loop 1
#define UB_BB_loop 5
#define LB_Branch_loop 1
#define UB_Branch_loop 5
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

typedef struct TestBench_Param {
	int N_branch;
	int N_loop;
	int N_BB_loop;
	int N_Branch_loop;
} testbench_param;

class testbench {
	private:
		int tasks_num;
		testbench_param *TaskCfg_param;

	public:
		testbench(
			int TskNum,                 // The number of tasks
			unsigned char branch_level, // The level of branch numbers, ranging from [LB_1,UB_1] to [LB_4,UB_4]
			unsigned char loop_level    // The level of loop numbers, ranging from [LB_1,UB_1] to [LB_4,UB_4]
		);
		~testbench();
		void param_show();
};

#endif // __PATTERN_GEN_H
