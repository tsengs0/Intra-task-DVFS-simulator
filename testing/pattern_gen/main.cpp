#include <iostream>
#include <vector>
#include "pattern_gen.h"
#include "parser.h"
#include "main.h"

using std::cout;
using std::endl;
using std::vector;

//-----------------------------------------------------------------------------------------//
// Functions
void system_init(void);
void wcet_info_config(void);
void checkpoint_config(void);
void TestPattern_config(void);
void array_int_cpy(vector<int> &Dst, int *Src);
void simulation_exe_path(Src_CFG &task, int path, float release_time, float start_time, int dvfs_en);
void func_gen(void *inout);
//-----------------------------------------------------------------------------------------//
//Input parameters
float in_alpha;
float in_default_speed;
double energy_ref;
char msg[41];
Parser parsing;
//-----------------------------------------------------------------------------------------//
//Parameters for simulation
int sim_cnt;
int sys_mode; // 1) H_RESP, 2) L_POWER
int test_case_1[] = {0, 5, 0, 5, 0, 5, 0, 5, 0, 5};
int test_case_2[] = {0, 0, 0, 0, 0, 5, 5, 5, 5, 5};
sys_clk_t Sys_Clk_0;
Time_Management *time_management;
//Task_State_Bus *inter_intra_bus;
extern float ISR_TIME_SLICE;
#define tasks_num  3 // The number of tasks 
int patterns_num;
vector<Src_CFG> src_intra;
//-----------------------------------------------------------------------------------------//
//Temporary test cases
typedef vector<int> ExePath_case;
typedef vector<ExePath_case> ExePath_set;
//-----------------------------------------------------------------------------------------//
//Checkpoints Objects
void checkpoint_config();
exeTime_info task_wcet_info_t[tasks_num] = {
	{
	 10450, // The worst-case execution cycle(s)
	 0    , // The average-case execution cycle(s)
	 1000   // The best-case exeucution cycle(s)
	},
	 
	{
	 12437, // The worst-case execution cycle(s)
	 0    , // The average-case execution cycle(s)
	 52     // The best-case exeucution cycle(s)
	},

	{
	 2000, // The worst-case execution cycle(s)
	 0   , // The average-case execution cycle(s)
	 250   // The best-case exeucution cycle(s)
	}
};
exeTime_info *task_wcet_info;
checkpoint_num *checkpoint_num_t; // The size will be known after parsing
RWCEC_Trace_in *cycle_trace; // The real size will be defined after parsing
checkpoints_label *checkpointLabel; // The label of checkpoints at each task's Basic Block(s)
ExePath_set *exe_path; // The set of exeuction-path set for tasks
// Three instaneces for each of those two tasks
int instance_case[2][3] = {
	{5, 3, 0}, // Task 1: exe_path_5(first instance), exe_path_3(second instance), exe_path_0(third instance)
	{5, 3, 1}  // Task 2: exe_path_5(first instance), exe_path_3(second instance), exe_path_1(third instance) 
};
//-----------------------------------------------------------------------------------------//
int main(int argc, char **argv)
{
	// For practicing the usage of "void pointer"
	//ExePath_case in;
	//func_gen((ExePath_case*) (&in)); 
	//for(int i = 0; i < in.size(); i++) cout << "in = " << in[i] << endl;
	if(argc != 3 && argc == 1) {
		cout << "The number of test patterns is required to be indicated" << endl;
		cout << "And the value of alpha is required to be set as well" << endl;
		exit(1);
	}
	else if(argc != 3 && argc == 2) {
		cout << "The value of alpha is required to be set as well" << endl;
		exit(1);
	}
	patterns_num = (int) atoi(argv[1]);
	in_alpha = ((float) atoi(argv[2])) / 100;
	in_default_speed = (float) 1000.0;//(float) atoi(argv[3]);
//=======================================================================================================================================================//
// Settings of each task's CFG information
	system_init();
	
	Src_CFG task1((char*) "task3.cfg", time_management, &checkpointLabel[0], &cycle_trace[0], &checkpoint_num_t[0], task_wcet_info[0]/*, exe_path[0]*/); 
	Src_CFG task2((char*) "task4.cfg", time_management, &checkpointLabel[1], &cycle_trace[1], &checkpoint_num_t[1], task_wcet_info[1]/*, exe_path[1]*/);
	Src_CFG task3((char*) "task5.cfg", time_management, &checkpointLabel[2], &cycle_trace[2], &checkpoint_num_t[2], task_wcet_info[3]/*, exe_path[2]*/); 
	src_intra.push_back(task1); src_intra.push_back(task2); src_intra.push_back(task3);
	cout << "The number of Intra-Source: " << src_intra.size() << endl;
	if(tasks_num != src_intra.size()) {
		cout << "The initial settings does not match Task-set pattern from input file" << endl;
		exit(1);	
	}
	TestPattern_config();
//=======================================================================================================================================================//
//	for(int i = 0; i < tasks_num; i++) {
//	 cout << "Task " << i + 1 << "'s test pattern:" << endl;
//	 for(int j = 0; j < src_intra[i].exe_path.size(); j++) {
//         cout << "Test Case " << j + 1 << ": " << endl;
//	  for(int k = 0; k < src_intra[i].exe_path[j].size(); k++) cout << "B" << src_intra[i].exe_path[j][k] << endl;
//	 }
//	}
//=======================================================================================================================================================//
	for(int tmp = 0; tmp < tasks_num; tmp++)
	for(int i = 0; i < patterns_num; i++) {
		src_intra[tmp].traverse_spec_path(i, (int) WORST, 0.0, 0.0, 50.0, (char) DVFS_ENABLE);
		cout << "==================================================================" << endl << endl;
	}
	
	return 0;
}

void func_gen(void *inout)
{
	ExePath_case &out = *(ExePath_case*) inout;

	for(int i = 0; i < 10; i++)
	out.push_back(i);
}
void system_init(void)
{
	srand((unsigned) time(0));
	energy_ref = 0.0;

	sys_mode = (int) H_RESP;
	Sys_Clk_0.cur_freq = 0.0; // Initially none of task is running
	Sys_Clk_0.cur_time  = 0.0;
	Sys_Clk_0.time_unit = (int) US;
	time_management = new Time_Management(Sys_Clk_0);
	
	checkpoint_config();
	wcet_info_config();
}

void checkpoint_config() {
	cycle_trace      = new RWCEC_Trace_in[tasks_num];
	checkpoint_num_t = new checkpoint_num[tasks_num];
	checkpointLabel  = new checkpoints_label[tasks_num];
	parsing.checkpoint_in(
			tasks_num,                             // The number of tasks 
			(RWCEC_Trace_in*) cycle_trace,         // The cycle tracing information for building Mining Table
			(checkpoint_num*) checkpoint_num_t,    // The number of eacc type heckpoints 
			(checkpoints_label*) checkpointLabel   // The label of checkpoints at each task's Basic Block(s)
	);
/*
	for(int i = 0; i < tasks_num; i++) {
		for(int j = 0; j < checkpointLabel[i].B_checkpoints.size(); j++) 
			cout << "B_checkpoint[" << j << "]: " << checkpointLabel[i].B_checkpoints[j] << endl;
		for(int k = 0; k < checkpointLabel[i].L_checkpoints.size(); k++)  
			for(int m = 0; m < checkpointLabel[i].L_checkpoints[k].size(); m++) 
				cout << "L_checkpoint[" << k << "][" << m << "]: " << checkpointLabel[i].L_checkpoints[k][m] << endl;
		for(int l = 0; l < checkpointLabel[i].P_checkpoints.size(); l++) 
			cout << "P_checkpoint[" << l << "]: " << checkpointLabel[i].P_checkpoints[l] << endl;
		for(int n = 0; n < checkpointLabel[i].L_loop_bound.size(); n++)
			cout << "L_bound[" << n << "]: " << checkpointLabel[i].L_loop_bound[n] << endl;	
		for(int n = 0; n < checkpointLabel[i].P_loop_bound.size(); n++)
			cout << "P_bound[" << n << "]: " << checkpointLabel[i].P_loop_bound[n] << endl;	
		cout << endl << endl;
	}
*/
}

void wcet_info_config() {
	task_wcet_info = new exeTime_info[tasks_num];
	memcpy(task_wcet_info, task_wcet_info_t, tasks_num * 3 * sizeof(int));
}

void TestPattern_config()
{
	exe_path = new ExePath_set[tasks_num];
 	for(int i = 0; i < tasks_num; i++) {
	 src_intra[i].P_loop_LaIteration = new int*[checkpointLabel[i].P_loop_bound.size()];
	 for(int j = 0; j < checkpointLabel[i].P_loop_bound.size(); j++) 
	  src_intra[i].P_loop_LaIteration[j] = new int[patterns_num];
	 rand_ExePath_gen (
	 	 src_intra[i].CFG_path,         // Pass each task's corrsponding Src_CFG
	 	 patterns_num,                  // The number of test patterns demanded to be generated
		 checkpointLabel[i],            // The label of checkpoints' corresponding Basic Block ID
		 (ExePath_set*) (&exe_path[i]), // The output of generated set of test patterns
		 (int**) src_intra[i].P_loop_LaIteration
	 );
	 for(int j = 0; j < checkpointLabel[i].P_loop_bound.size(); j++) {
	   for(int k = 0; k < patterns_num; k++)
	    cout << "Task" << i << " " << j << "th's P-ch, case" << k << ": " << src_intra[i].P_loop_LaIteration[j][k] << endl; 
	 }
	 src_intra[i].pattern_init(exe_path[i]);
	}
}

void array_int_cpy(vector<int> &Dst, int *Src)
{
	int a = 0;
	for(a = 0; Src[a] != 0x7FFFFFFF; a++) Dst.push_back(Src[a]);
}
