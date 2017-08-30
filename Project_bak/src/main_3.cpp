#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "../inc/cfg_info.h"
#include "../inc/main.h"
#include "../inc/dvfs_info.h"
#include "../inc/rt_simulator.h"

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
int exe_path_4[] = {1, 2, 4, 1, 2, 3, 4, 1, 5, 7, 0x7FFFFFFF};
int exe_path_5[] = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 5, 6, 7, 0x7FFFFFFF};
int B_checkpoints_1[] = {5, 0x7FFFFFFF};
int L_checkpoints_1[] = {1, 2, 0x7FFFFFFF};
checkpoints_t checkpoints_1;
//-----------------------------------------------------------------------------------------//
//Input parameters
float in_alpha;
float in_default_speed;
char msg[41];
//-----------------------------------------------------------------------------------------//
//Parameters for simulation
int sim_cnt;
int alpha_global = 4;
int sys_mode; // 1) H_RESP, 2) L_POWER
int test_case_1[] = {0, 5, 0, 5, 0, 5, 0, 5, 0, 5};
int test_case_2[] = {0, 0, 0, 0, 0, 5, 5, 5, 5, 5};
sys_clk_t Sys_Clk_0;
sys_clk_t Sys_Clk_1;
sys_clk_t Sys_Clk_2;
sys_clk_t Sys_Clk_3;
sys_clk_t Sys_Clk_4;
Time_Management *time_management;
Time_Management *time_management1;
Time_Management *time_management2;
Time_Management *time_management3;
Time_Management *time_management4;
//-----------------------------------------------------------------------------------------//
int main(int argc, char **argv)
{
	float release_time = 0.0, start_time = 0.0;

	system_init();

	in_alpha = ((float) atoi(argv[2])) / 100;
	in_default_speed = ((float) atoi(argv[3])) / 1000; // Hz -> KHz
#ifndef DEBUG
	sprintf(msg, "echo \"alpha = %.02f\" > test_result%.02f_%.01f.txt", in_alpha, in_alpha, in_default_speed);
	system(msg);
#endif
	//Src_CFG task1(argv[1], time_management, checkpoints_1);
	Src_CFG task2(argv[1], time_management1, checkpoints_1, exe_path);
	Src_CFG task3(argv[1], time_management2, checkpoints_1, exe_path);
	Src_CFG task4(argv[1], time_management3, checkpoints_1, exe_path);
	Src_CFG task5(argv[1], time_management4, checkpoints_1, exe_path);
	
		for(int i = 0; i < 4; i++) {
			int case_t = (i == 0) ? 0 : 
				     (i == 3) ? 5 : rand() % 6;
			//int case_t = (rand() % 4) + 1;

			int start_offset = rand() % (2500 - 2420);
			start_time = release_time + (float) start_offset;
			/*
			cout << "Instance_" << i << " without DVFS" << endl;
			alpha_global = 1;
			simulation_exe_path(task1, case_t, release_time, start_time, DVFS_DISABLE);
			*/
			
			/*
			cout << "----------------------------------------------------------" << endl;
			cout << "Instance_" << i << " with approach 1" << endl;
			alpha_global = 4;
			//simulation_exe_path(task2, case_t, release_time, start_time, DVFS_ENABLE);
			task2.traverse_spec_path(case_t, WORST, release_time, start_time - (float) start_offset, 2500, (char) DVFS_ENABLE); 
			*/

			/*
			cout << "----------------------------------------------------------" << endl;
			cout << "Instance_" << i << " with approach 2" << endl;
			alpha_global = 4;
			//simulation_exe_path(task3, case_t, release_time, start_time, DVFS_ENABLE);
			task3.traverse_spec_path(case_t, WORST, release_time, start_time - (float) start_offset, 2500, (char) DVFS_ENABLE); 
			*/

			cout << "----------------------------------------------------------" << endl;
			cout << "Instance_" << i << " with approach 3" << endl;
			alpha_global = 4;
			//simulation_exe_path(task4, case_t, release_time, start_time, DVFS_ENABLE);
			task4.traverse_spec_path(case_t, WORST, release_time, start_time + (float) start_offset, 2500, (char) DVFS_ENABLE); 
			
			/*
			cout << "----------------------------------------------------------" << endl;
			cout << "Instance_" << i << " with approach 4" << endl;
			alpha_global = 4;
			//simulation_exe_path(task5, case_t, release_time, start_time, DVFS_ENABLE);
			task5.traverse_spec_path(case_t, WORST, release_time, start_time, 2500, (char) DVFS_ENABLE); 
			*/
			cout << "----------------------------------------------------------" << endl;
			release_time += 2420; // 2420 ms
			//task1.L_loop_iteration.at(0) = 3 + 1;
			task2.L_loop_iteration.at(0) = 3 + 1;
			task3.L_loop_iteration.at(0) = 3 + 1;
			task4.L_loop_iteration.at(0) = 3 + 1;
			task5.L_loop_iteration.at(0) = 3 + 1;
		}
#ifndef DEBUG
		//task1.output_result((char*) "(non-DVFS)");
		task2.output_result((char*) "(1)"); // without changing value of alpha
		task3.output_result((char*) "(2)"); // changing value of alpha by average case
		task4.output_result((char*) "(3)"); // changing value of alpha by approach 3
		task5.output_result((char*) "(4)"); // changing value of alpha by approach 4
#endif

		delete time_management;

	return 0;
}

void system_init(void)
{
	int i;
	vector<int> L_ch_temp;
	vector<int> exe_path_temp;

	srand((unsigned) time(0));

	sys_mode = (int) H_RESP;
	Sys_Clk_0.cur_freq = 0.0; // Initially none of task is running
	Sys_Clk_0.cur_time  = 0.0;
	Sys_Clk_0.time_unit = (int) MS;
	time_management = new Time_Management(Sys_Clk_0);
	
	Sys_Clk_1.cur_freq = 0.0; // Initially none of task is running
	Sys_Clk_1.cur_time  = 0.0;
	Sys_Clk_1.time_unit = (int) MS;
	time_management1 = new Time_Management(Sys_Clk_1);
	
	Sys_Clk_2.cur_freq = 0.0; // Initially none of task is running
	Sys_Clk_2.cur_time  = 0.0;
	Sys_Clk_2.time_unit = (int) MS;
	time_management2 = new Time_Management(Sys_Clk_2);
	
	Sys_Clk_3.cur_freq = 0.0; // Initially none of task is running
	Sys_Clk_3.cur_time  = 0.0;
	Sys_Clk_3.time_unit = (int) MS;
	time_management3 = new Time_Management(Sys_Clk_3);
	
	Sys_Clk_4.cur_freq = 0.0; // Initially none of task is running
	Sys_Clk_4.cur_time  = 0.0;
	Sys_Clk_4.time_unit = (int) MS;
	time_management4 = new Time_Management(Sys_Clk_4);

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

	array_int_cpy(exe_path_temp, exe_path_0); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF);exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_1); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF);exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_2); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF);exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_3); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF);exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_4); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF);exe_path_temp.clear();
	array_int_cpy(exe_path_temp, exe_path_5); exe_path.push_back(exe_path_temp); exe_path.back().push_back(0x7FFFFFFF);exe_path_temp.clear();
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
