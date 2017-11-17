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
#include "../Parser/parser.h"
#include "../inc/pattern_gen.h"

using std::cout;
using std::cin;
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
sys_clk_t Sys_Clk_0;
Time_Management *time_management;
Task_State_Bus *inter_intra_bus;
extern float ISR_TIME_SLICE;
//int tasks_num = 3; // The number of tasks 
//int patterns_num = 1;
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
         9750, // The worst-case execution cycle(s)
         0    , // The average-case execution cycle(s)
         1000   // The best-case exeucution cycle(s)
        },

        {
         11950, // The worst-case execution cycle(s)
         0    , // The average-case execution cycle(s)
         52     // The best-case exeucution cycle(s)
        },

        {
         1810, // The worst-case execution cycle(s)
         0   , // The average-case execution cycle(s)
         260   // The best-case exeucution cycle(s)
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
	if(argc != 3 && argc == 1) {
		cout << "The Default Operating Frequency ought to be defined" << endl;
		exit(1);
	}
	else if(argc != 3 && argc == 0) {
		cout << "The both 1) value of alpha(0~100) and 2) Default Operating Frequency ought to be defined" << endl;
		exit(1);
	}
	else {
		in_alpha = ((float) atoi(argv[1])) / 100;
		in_default_speed = (float) atoi(argv[2]);
	}
//=======================================================================================================================================================//
// Settings of each task's CFG information
	cout << "Starting to initialise system" << endl;
	system_init();
	cout << "After system initialisation" << endl;
	Src_CFG task1((char*) "../cfg/task3.cfg", time_management, &checkpointLabel[0], &cycle_trace[0], &checkpoint_num_t[0], task_wcet_info[0]/*, exe_path[0]*/); cout << "Finished configuring task1" << endl;
	Src_CFG task2((char*) "../cfg/task4.cfg", time_management, &checkpointLabel[1], &cycle_trace[1], &checkpoint_num_t[1], task_wcet_info[1]/*, exe_path[1]*/); cout << "Finished configuring task2" << endl;
	Src_CFG task3((char*) "../cfg/task5.cfg", time_management, &checkpointLabel[2], &cycle_trace[2], &checkpoint_num_t[2], task_wcet_info[2]/*, exe_path[2]*/); cout << "FInished configuring task3" << endl;
	src_intra.push_back(task1); src_intra.push_back(task2); src_intra.push_back(task3);
	cout << "The number of Intra-Source: " << src_intra.size() << endl;
	if(tasks_num != src_intra.size()) {
		cout << "The initial settings does not match Task-set pattern from input file" << endl;
		exit(1);	
	}
	cout << "Starting to configure test patterns for all tasks" << endl;
	TestPattern_config();
	cout << "Finished configuring settings of test patterns" << endl;
//=======================================================================================================================================================//
// Settings of Inter-task	
	cout << "Starting to configure Inter tasks" << endl;
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
	cout << "Finished configuring Inter tasks" << endl;
//=======================================================================================================================================================//
// Settings of Intra- and Inter-task communication Bus and Task Management
	cout << "Starting to bind each Intra task(Control Flow Information) to their corresponding Inter task" << endl;
	inter_intra_bus = new Task_State_Bus(time_management, src_inter, src_intra);
	Task_Scheduler task_sched(time_management, src_inter, que, (char) RM, inter_intra_bus);
	cout << "Finished binding Intra tasks with Inter tasks" << endl;
//=======================================================================================================================================================//
// Setting the Jitter constraints
	cout << "Starting to set jitter contraints" << endl;
	for(int i = 0; i < tasks_num; i++) inter_intra_bus -> intra_tasks[i].jitter_init(); 	
	cout << "Finished setting jitter constraints" << endl;
//=======================================================================================================================================================//
	cout << "==================================================" << endl;
	cout << "\t\t";
	for(int i = 0; i < tasks_num; i++) cout << "task_" << i << "\t";
	cout << endl << "--------------------------------------------------" << endl;
	time_management -> update_cur_time(0.0);
	task_sched.sched_arbitration(0.000);
	cout << "0 us - " << endl;
	float cur_time;
	for(cur_time = 0.001; /*time_management -> sys_clk -> cur_time <= 100.0*/; ) {
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
		cout << endl << time_management -> sys_clk -> cur_time << " us\t\t";
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
        for(unsigned int i = 0; i < tasks_num; i++) {
         src_intra[i].P_loop_LaIteration = new int*[checkpointLabel[i].P_loop_bound.size()];
         for(unsigned int j = 0; j < checkpointLabel[i].P_loop_bound.size(); j++)
          src_intra[i].P_loop_LaIteration[j] = new int[patterns_num];
         rand_ExePath_gen (
                 src_intra[i].CFG_path,         // Pass each task's corrsponding Src_CFG
                 patterns_num,                  // The number of test patterns demanded to be generated
                 checkpointLabel[i],            // The label of checkpoints' corresponding Basic Block ID
                 (ExePath_set*) (&exe_path[i]), // The output of generated set of test patterns
                 (int**) src_intra[i].P_loop_LaIteration
         );
         for(unsigned int j = 0; j < checkpointLabel[i].P_loop_bound.size(); j++) {
           for(unsigned int k = 0; k < patterns_num; k++)
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

