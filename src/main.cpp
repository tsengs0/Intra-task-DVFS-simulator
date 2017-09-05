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

using namespace std;

//-----------------------------------------------------------------------------------------//
// Functions
void system_init(void);
void array_int_cpy(vector<int> &Dst, int *Src);
void simulation_exe_path(Src_CFG &task, int path, float release_time, float start_time, int dvfs_en);
//-----------------------------------------------------------------------------------------//
//Temporary test cases
vector< vector<int> > exe_path;
int exe_path_0[] = {1, 5, 7, 0x7FFFFFFF};
int exe_path_1[] = {1, 2, 3,4,1,5,6,7, 0x7FFFFFFF};
int exe_path_2[] = {1, 2, 4, 1, 5, 6, 0x7FFFFFFF};
int exe_path_3[] = {1, 2, 4, 1, 5, 7, 0x7FFFFFFF};
int exe_path_4[] = {1, 2, 4, 1, 2, 3, 4, 1, 2, 4, 1, 5, 6, 7, 0x7FFFFFFF};
int exe_path_5[] = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 5, 6, 7, 0x7FFFFFFF};
int B_checkpoints_1[] = {5, 0x7FFFFFFF};
int L_checkpoints_1[] = {1, 2, 0x7FFFFFFF};
checkpoints_t checkpoints_1;
int task_wcet_info[2][3] = {{605, 80, 75}, {425, 80, 30}};

int B_RWCEC_1[1][4] = {
	//{2, 190, 5, 75}, // the index of successor, Taken, the index of successor, Not Taken
	{7, 20, 6, 70}
};
int B_RWCEC_2[1][4] = {
	//{2, 190, 5, 75}, // the index of successor, Taken, the index of successor, Not Taken
	{7, 20, 6, 70}
};
/*static int B_RWCEC_t[2][4] = {
	{2, 235, 5, 75}, // the index of successor, Taken, the index of successor, Not Taken
	{6, 70, 7, 20},
};*/

// One loop nest; one L-type checkpoint indside; 6 informations about RWCEC
int L_RWCEC_1[1][2][8] = {
//  entrance or not, successor_1, iteration_1, iteration_2, iteration_3; successor_2, iteration_1, iteration_2, iteration_3	
	{{5, 75, 75, 75, 2, 235, 395, 555}, {4, 130, 290, 450, 3, 230, 390, 550}}
};
int L_RWCEC_2[1][2][8] = {
//  entrance or not, successor_1, iteration_1, iteration_2, iteration_3; successor_2, iteration_1, iteration_2, iteration_3	
	{{5, 75, 75, 75, 2, 190, 305, 420}, {4, 85, 200, 315, 3, 185, 300, 415}}
};
RWCEC_Trace_in cycle_trace_1, cycle_trace_2;
int instance_case[2][3] = {
	{5, 3, 0},
	{5, 3, 1}
};
int instance_index[2] = {0,0};
//-----------------------------------------------------------------------------------------//
//Input parameters
float in_alpha;
float in_default_speed;
char msg[41];
double energy_ref;
//-----------------------------------------------------------------------------------------//
//Parameters for simulation
int sim_cnt;
int alpha_global = 3;
int sys_mode; // 1) H_RESP, 2) L_POWER
int test_case_1[] = {0, 5, 0, 5, 0, 5, 0, 5, 0, 5};
int test_case_2[] = {0, 0, 0, 0, 0, 5, 5, 5, 5, 5};
sys_clk_t Sys_Clk_0;
Time_Management *time_management;
Task_State_Bus *inter_intra_bus;
extern float ISR_TIME_SLICE;
//-----------------------------------------------------------------------------------------//
int main(int argc, char **argv)
{
//=======================================================================================================================================================//
// Settings of each task's CFG information
	float release_time = 0.0, start_time = 0.0;

	system_init();

	in_alpha = (float) 50.0; //((float) atoi(argv[2])) / 100;
	in_default_speed = (float) 1000.0;//(float) atoi(argv[3]);

	vector<Src_CFG> src_intra;
	Src_CFG task1((char*) "../cfg/task1.cfg", time_management, checkpoints_1, &cycle_trace_1, task_wcet_info[0], exe_path); 
	Src_CFG task2((char*) "../cfg/task2.cfg", time_management, checkpoints_1, &cycle_trace_2, task_wcet_info[1], exe_path);
	src_intra.push_back(task1); src_intra.push_back(task2);
	alpha_global = 3;
	cout << "The number of Intra-Source: " << src_intra.size() << endl;
	
//=======================================================================================================================================================//
// Settings of Inter-task
	vector<task_info_t> src_inter;
	//sys_clk_t sys_clk;
	Ready_Queue que;
	//Time_Management time_management(sys_clk);
	src_inter.push_back({0.0, /*start_time*/0.0, 1, /*Period: 3.0 us*/3.0, 605, 3.0, false, (char) ZOMBIE});
	src_inter.push_back({0.0, /*start_time*/0.0, 0, /*Period: 1.0 us*/1.0, 425, 1.0, false, (char) ZOMBIE});
//=======================================================================================================================================================//
// Settings of Intra- and Inter-task communication Bus and Task Management
	inter_intra_bus = new Task_State_Bus(time_management, &src_inter, &src_intra);

	Task_Scheduler task_sched(time_management, src_inter, que, (char) RM, inter_intra_bus);
	cout << "task_list size: " << task_sched.task_list.size() << " " << src_inter.size() << endl;
//=======================================================================================================================================================//	
	cout << "==================================================" << endl;
	cout << "\t\t";
	for(int i = 0; i < src_inter.size(); i++) cout << "task_" << i << "\t";
	cout << endl << "--------------------------------------------------" << endl;
	time_management -> update_cur_time(0.0);
	task_sched.sched_arbitration(0.000);
	cout << "0 us - " << endl;
	float cur_time;
	for(cur_time = 0.001; time_management -> sys_clk -> cur_time <= 3.0; ) {
		for(int i = 0; i < src_inter.size(); i++) { 
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
	
	// Accroding to the Input file
	//checkpoints_1.B_checkpoints = (int*) malloc(sizeof(int) * 2);
	//checkpoints_1.B_checkpoints =/&B_checkpoints_1[0];//memcpy(checkpoints_1.B_checkpoints, (int[3]){1, 5, 0x7FFFFFFF}, sizeof(int[3]));
	array_int_cpy(checkpoints_1.B_checkpoints, B_checkpoints_1);

	// Accroding to the Input file
	//checkpoints_1.L_checkpoints = (int**) malloc(sizeof(int*) * 1); // The number of Loop nest is 1  
	//checkpoints_1.L_checkpoints[0] = (int*) malloc(sizeof(int) * 3); // There are two checkpoints inside current loop nest
	//checkpoints_1.L_checkpoints[0] = &L_checkpoints_1[0];//memcpy(checkpoints_1.L_checkpoints[0], (int[2])(2, 0x7FFFFFFF), sizeof(int[2]));
	array_int_cpy(L_ch_temp, L_checkpoints_1);
	checkpoints_1.L_checkpoints.push_back(L_ch_temp); vector<int>().swap(L_ch_temp);
	checkpoints_1.L_loop_iteration.push_back(3 + 1);

	array_int_cpy(exe_path_temp, exe_path_0); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF); exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_1); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF); exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_2); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF); exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_3); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF); exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_4); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF); exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_5); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF); exe_path_temp.clear();

	for(int i = 0; i < 1; i++) {
	 for(int j = 0; j < 4; j++) {
	 	cycle_trace_1.B_RWCEC_t[i][j] = B_RWCEC_1[i][j];
	 	cycle_trace_2.B_RWCEC_t[i][j] = B_RWCEC_2[i][j];
	 }
	}
	for(int i = 0; i < 1; i++) 
	 for(int j = 0; j < 2; j++)
	  for(int k = 0; k < 8; k++) {
	  	cycle_trace_1.L_RWCEC_t[i][j][k] = L_RWCEC_1[i][j][k];
	  	cycle_trace_2.L_RWCEC_t[i][j][k] = L_RWCEC_2[i][j][k];
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
