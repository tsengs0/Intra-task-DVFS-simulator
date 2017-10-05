#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "../inc/cfg_info.h"
#include "../inc/main.h"
#include "../inc/dvfs_info.h"
//#include "../inc/rt_simulator.h"
#include "../inc/sched.h"
#include "../inc/timer.h"
#include "../inc/inter_bus.h"
#include "../inc/checkpoint_info.h"

using std::cout;
using std::cin;
using std::endl;
using std::vector;

//-----------------------------------------------------------------------------------------//
// Functions
void system_init(void);
void array_int_cpy(vector<int> &Dst, int *Src);
void simulation_exe_path(Src_CFG &task, int path, float release_time, float start_time, int dvfs_en);
//-----------------------------------------------------------------------------------------//
//Input parameters
float in_alpha;
float in_default_speed;
char msg[41];
double energy_ref;
//-----------------------------------------------------------------------------------------//
//Parameters for simulation
int sim_cnt;
int sys_mode; // 1) H_RESP, 2) L_POWER
int test_case_1[] = {0, 5, 0, 5, 0, 5, 0, 5, 0, 5};
int test_case_2[] = {0, 0, 0, 0, 0, 5, 5, 5, 5, 5};
sys_clk_t Sys_Clk_0;
Time_Management *time_management;
Task_State_Bus *inter_intra_bus;
extern float ISR_TIME_SLICE;
int tasks_num = 3; // The number of tasks 
//-----------------------------------------------------------------------------------------//
//Temporary test cases
vector< vector<int> > exe_path;
int exe_path_0[] = {1, 5, 7, 0x7FFFFFFF};
int exe_path_1[] = {1, 2, 3,4,1,5,6,7, 0x7FFFFFFF};
int exe_path_2[] = {1, 2, 4, 1, 5, 6, 0x7FFFFFFF};
int exe_path_3[] = {1, 2, 4, 1, 5, 7, 0x7FFFFFFF};
int exe_path_4[] = {1, 2, 4, 1, 2, 3, 4, 1, 2, 4, 1, 5, 6, 7, 0x7FFFFFFF};
int exe_path_5[] = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 5, 6, 7, 0x7FFFFFFF};
//-----------------------------------------------------------------------------------------//
//Checkpoints Objects
void checkpoint_config();
int *B_checkpoints_1;
int *L_checkpoints_1;
int P_checkpoints_1[] = { // P-type[1] or LaVSP[2]
	1
}; 
checkpoints_t checkpoints_1;

int *B_checkpoints_2;
int *L_checkpoints_2;
int P_checkpoints_2[] = {
	2,
	4, 
	6 
};
checkpoints_t checkpoints_2;

int B_checkpoints_3[] = {
	5 
};
int *L_checkpoints_3;
int P_checkpoints_3[] = {
	1 
};
checkpoints_t checkpoints_3;

int task_wcet_info[tasks_num][3] = {
	{
	 10450, // The worst-case execution cycle(s)
	 0    , // The average-case execution cycle(s)
	 0      // The best-case exeucution cycle(s)
	},
	 
	{
	 12437, // The worst-case execution cycle(s)
	 0    , // The average-case execution cycle(s)
	 0      // The best-case exeucution cycle(s)
	},

	{
	 2000, // The worst-case execution cycle(s)
	 0   , // The average-case execution cycle(s)
	 0     // The best-case exeucution cycle(s)
	}
};

checkpoint_num checkpoint_num_t[tasks_num];
// All the value = 0 means "No B-type branch instruction exists"

checkpoint_num_t[1] = {
	0, // Number of B-type checkpoints 
	0, // Number of L-type checkpoints
	3  // Number of P-type checkpoints
};

int P_RWCEC_1[1][1][3] = {
int P_RWCEC_1[1][1][3] = {
int B_RWCEC_3[1][4] = {
	{7, 100, 6, 300}
};
int P_RWCEC_3[1][1][3] = {
 {
	// Loop_1:
	// Loop Entry: Basic Block 1
	{
	 5,   // Loop Bound
	 310, // WCEC(cycles) within this loop
	 400, // Inherent RWCEC(cycles) after current basic block
	}
 }
};
checkpoint_num_t[2] = {
	1, // Number of B-type checkpoints 
	0, // Number of L-type checkpoints
	1  // Number of P-type checkpoints
};

RWCEC_Trace_in cycle_trace[tasks_num];
int instance_case[2][3] = {
	{5, 3, 0},
	{5, 3, 1}
};
int instance_index[2] = {0,0};
//-----------------------------------------------------------------------------------------//
int main(int argc, char **argv)
{
//=======================================================================================================================================================//
// Settings of each task's CFG information
	float release_time = 0.0, start_time = 0.0;
	system_init();
	
	in_alpha = ((float) atoi(argv[2])) / 100;
	in_default_speed = (float) 1000.0;//(float) atoi(argv[3]);
	
	vector<Src_CFG> src_intra;
	Src_CFG task1((char*) "../cfg/task3.cfg", time_management, checkpoints_1, &cycle_trace[0], task_wcet_info[0], exe_path); 
	Src_CFG task2((char*) "../cfg/task4.cfg", time_management, checkpoints_2, &cycle_trace[1], task_wcet_info[1], exe_path);
	Src_CFG task3((char*) "../cfg/task5.cfg", time_management, checkpoints_3, &cycle_trace[2], task_wcet_info[3], exe_path); 
	src_intra.push_back(task1); src_intra.push_back(task2); src_intra.push_back(task3);
	cout << "The number of Intra-Source: " << src_intra.size() << endl;
	if(tasks_num != src_intra.size()) {
		cout << "The initial settings does not match Task-set pattern from input file" << endl;
		exit(1);	
	}
//=======================================================================================================================================================//
// Settings of Inter-task
	Ready_Queue que;
	task_info_t *src_inter = new task_info_t[tasks_num];
	src_inter[0] = {
			     0.0, // Release Time
			     0.0, // Start Time 
			     0  , // Priority
			     15.0, // Relative Deadline
			     task1.wcet,//task1.wcet, // WCET
		             24.0, // Period
			     false, 
			     (char) ZOMBIE, // Default Task State
			     task1.wcet // Default WCRT
	};

	src_inter[1] = {
			     0.0, // Release Time
			     0.0, // Start Time 
			     1  , // Priority
			     25.0, // Relative Deadline
			     task2.wcet, // WCET
		             35.0, // Period
			     false, 
			     (char) ZOMBIE, // Default Task State
			     task2.wcet // Default WCRT
	};
	
	src_inter[2] = {
			     0.0, // Release Time
			     0.0, // Start Time 
			     2  , // Priority
			     50.0, // Relative Deadline
			     task3.wcet, // WCET
		             55.0, // Period
			     false, 
			     (char) ZOMBIE, // Default Task State
			     task3.wcet // Default WCRT
	};
//=======================================================================================================================================================//
// Settings of Intra- and Inter-task communication Bus and Task Management
	inter_intra_bus = new Task_State_Bus(time_management, src_inter, src_intra);
	Task_Scheduler task_sched(time_management, src_inter, que, (char) RM, inter_intra_bus);
//=======================================================================================================================================================//
// Setting the Jitter constraints
	for(int i = 0; i < tasks_num; i++) inter_intra_bus -> intra_tasks[i].jitter_init(); 	
//=======================================================================================================================================================//
	cout << "==================================================" << endl;
	cout << "\t\t";
	for(int i = 0; i < tasks_num; i++) cout << "task_" << i << "\t";
	cout << endl << "--------------------------------------------------" << endl;
	time_management -> update_cur_time(0.0);
	task_sched.sched_arbitration(0.000);
	cout << "0 us - " << endl;
	float cur_time;
	for(cur_time = 0.001; time_management -> sys_clk -> cur_time <= 3.0; ) {
		for(int i = 0; i < tasks_num; i++) { 
			if(task_sched.task_list[i].state == (char) RUN) {
				inter_intra_bus -> time_driven_cfg(i);
				for(int j = 0; j < 15; j++) cout << "-"; 
				for(int j = 0; j < 8*i; j++) cout << "-"; 
				cout << "|" << i << "|";
			}
		} 
		//time_management -> update_cur_time(cur_time);
		task_sched.sched_arbitration(cur_time);
		cout << endl << time_management -> sys_clk -> cur_time << " us*\t\t";
		//task_sched.list_task_state();
		cout << endl; // << "--------------------------------------------------" << endl;
		if(task_sched.IsIdle() == true) {
			// Extracting decimal point(s) and doing accumulative addition by the number of 0.001 
			cur_time = (time_management -> sys_clk -> cur_time) + 0.001;
			cur_time = (int) (cur_time * N_DECIMAL_POINTS_PRECISION);
			cur_time = (float) ((cur_time + 1.0) / N_DECIMAL_POINTS_PRECISION); 
			time_management -> update_cur_time(cur_time);
		}
		else cur_time += 0.001;	
	}
	cout << "==================================================" << endl;

	delete time_management;
		
	return 0;
}

void system_init(void)
{
	int i;
	
	vector<int> L_ch_temp;
	vector<int> exe_path_temp;

	srand((unsigned) time(0));
	energy_ref = 0.0;

	sys_mode = (int) H_RESP;
	Sys_Clk_0.cur_freq = 0.0; // Initially none of task is running
	Sys_Clk_0.cur_time  = 0.0;
	Sys_Clk_0.time_unit = (int) US;
	time_management = new Time_Management(Sys_Clk_0);
	
	array_int_cpy(checkpoints_1.B_checkpoints, B_checkpoints_1);

	array_int_cpy(L_ch_temp, L_checkpoints_1);
	checkpoints_1.L_checkpoints.push_back(L_ch_temp); vector<int>().swap(L_ch_temp);
	checkpoints_1.L_loop_iteration.push_back(3 + 1);

	array_int_cpy(exe_path_temp, exe_path_0); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF); exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_1); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF); exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_2); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF); exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_3); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF); exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_4); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF); exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_5); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF); exe_path_temp.clear();

	checkpoint_config();

}

void checkpoint_config() {

int *B_RWCEC_1;
int ***L_RWCEC_1;
int P_RWCEC_1[1][1][3] = {
 {
	// Loop_1: 
	// Loop Entry: Basic Block 1 
	{
	 7,    // Loop Bound
	 1350, // WCEC(cycles) within this loop 
	 800   // Inherent RWCEC(cycles) after current basic block
	}
 }
};
checkpoint_num_t[0] = {
	0, // Number of B-type checkpoints 
	0, // Number of L-type checkpoints
	1  // Number of P-type checkpoints
};

int *B_RWCEC_2;
int ***L_RWCEC_2;
int P_RWCEC_2[1][3][3] = {
 {
	// Loop_1:
	// Loop Entry: Basic Block 2
	// Function Call: my_cos()
	{
	 150,  // Loop Bound
	 30,   // WCEC(cycles) within this loop
	 7934, // Inherent RWCEC(cycles) after current basic block
	},
	
	// Loop_2:
	// Loop Entry: Basic Block 4
	// Function Call: encode()
	{
	 100,  // Loop Bound
	 40,   // WCEC(cycles) within this loop
	 3919, // Inherent RWCEC(cycles) after current basic block
	},
	
	// Loop_3:
	// Loop Entry: Basic Block 6
	// Function Call: decode()
	{
	 130, // Loop Bound
	 30,  // WCEC(cycles) within this loop
	 12,  // Inherent RWCEC(cycles) after current basic block
	}
 }
};
	for(int i = 0; i < tasks_num; i++) {
		if(checkpoint_num[i].B_ch != 0) {
			cycle_trace[i].B_RWCEC_1[0] = B_RWCEC_1[0];
			cycle_trace[i].B_RWCEC_1[1] = B_RWCEC_1[1];
			cycle_trace[i].B_RWCEC_1[2] = B_RWCEC_1[2];
			cycle_trace[i].B_RWCEC_1[3] = B_RWCEC_1[3];
		}
		
		if(checkpoint_num[i].L_ch != 0) {
			cycle_trace[]
		}
		
		if(checkpoint_num[i].P_ch != 0) {

		}
	}
}
void array_int_cpy(vector<int> &Dst, int *Src)
{
	int a = 0;
	for(a = 0; Src[a] != 0x7FFFFFFF; a++) Dst.push_back( Src[a] );
}

/*
void simulation_exe_path(Src_CFG &task, int path, float release_time, float start_time, int dvfs_en)
{
	switch(path) {
		case 0:
			task.traverse_spec_path(exe_path_0, WORST, release_time, start_time, dvfs_en); 
			break;
		case 1:
			task.traverse_spec_path(exe_path_1, WORST, release_time, start_time, dvfs_en); 
			break;
		case 2:
			task.traverse_spec_path(exe_path_2, WORST, release_time, start_time, dvfs_en);
			break; 
		case 3:
			task.traverse_spec_path(exe_path_3, WORST, release_time, start_time, dvfs_en);
			break; 
		case 4:
			task.traverse_spec_path(exe_path_4, WORST, release_time, start_time, dvfs_en);
			break; 
		case 5:
			task.traverse_spec_path(exe_path_5, WORST, release_time, start_time, dvfs_en);
			break; 
		
	}
}
*/
