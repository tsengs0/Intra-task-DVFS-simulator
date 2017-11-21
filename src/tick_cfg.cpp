#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../inc/cfg_info.h"
#include "../inc/dvfs_info.h"
#include "../inc/main.h"

using std::cin;
using std::cout;
using std::endl;
using std::vector;

int cur_TskID;
extern float in_alpha;
extern float in_default_speed;
extern int sim_cnt;
extern int sys_mode;
extern double energy_ref;
extern float ISR_TIME_SLICE;

/**
  * @brief Executing/simulating the sub-portion of control flow graph in terms of 
  *	   interrupt timer's period(preloaded cycle count). For sake of simulating 
  *        preemptive behaiour
  * @param case_t: designating the estimated 1)worst-case, 2)average-case, 
  *        3)best-case to entire execution
  * @param DVFS_en: enable or disable DVFS mechanism  
**/   
isr_context_t Src_CFG::isr_driven_cfg(int case_t, char DVFS_en)
{
	int cur_index, cycles_cnt = 0;
	int rem_block_cycles, sub_block_cycles, isr_time_cycles;
	int rem_cycles_temp;
	dvfs_en = DVFS_en;
	isr_context_t context_reg;

//----------------------------------------------------------------------------------------------------------------//
	// Remaining execution cycles of current basic block	
	rem_block_cycles = CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].get_cycles(case_t) - executed_cycles; 	
	
	// Canonical number of execution cycles which will be taken within one period of interrupt timer	
	isr_time_cycles = (int) (((float) ISR_TIME_SLICE) * ((float) (time_management -> sys_clk -> cur_freq)));
	
	sub_block_cycles = (isr_time_cycles % (int) INST_UNIT == 0) ? isr_time_cycles : isr_time_cycles - (isr_time_cycles % (int) INST_UNIT);	
	rem_cycles_temp = sub_block_cycles - rem_block_cycles; 
	if(rem_cycles_temp > 0) { // Crossing multiple basic blocks within one period of interrupt timer
#ifdef DVFS_EN  // Before crossing to next basic block, checking if current basic block is a checkpoint
			if(dvfs_en == (char) DVFS_ENABLE) {
				// Invoking the operation of B-type checkpoint
				if(CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].B_checkpoint_en != 0x7FFFFFFF) {
					B_Intra_task_checkpoint(
						exe_path[cur_case_id][cur_block_index],    // Cast current Basic Block ID 
						exe_path[cur_case_id][cur_block_index + 1] // Cast its successive Basic Block ID according to the indicated execution path case
					);		
				}
				// Invoking the operation of L-type checkpoint
				else if(CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].L_checkpoint_en[0] != 0x7FFFFFFF) { cout << "LLLLLLLL" << endl;
					L_Intra_task_checkpoint(
						exe_path[cur_case_id][cur_block_index],    // Cast current Basic Block ID
						exe_path[cur_case_id][cur_block_index + 1] // Cast its successive Basic Block ID according ot the indicated execution path case
					);
				}			
				else if(CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].P_checkpoint_en != 0x7FFFFFFF) { 
					P_Intra_task_checkpoint(
						exe_path[cur_case_id][cur_block_index],    // Cast current Basic Block ID 
						exe_path[cur_case_id][cur_block_index + 1] // Cast its successive Basic Block ID according to the indicated execution path case
					);		
				}
				else {
				}	
			}	
#endif 
		cur_block_index += 1;			
		while(CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].get_cycles(case_t) < rem_cycles_temp) { 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
#ifdef DEBUG
			printf("[Cur_Freq: %.01f MHz]", time_management -> sys_clk -> cur_freq);
			cout << "Block_" << CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].get_index() << " -> ";
			for(int j = 0; j < 15; j++) cout << "-"; 
			for(int j = 0; j < 8*cur_TskID; j++) cout << "-"; 
			cout << "|" << cur_TskID << "|";
#endif
			/*
			cycles_cnt += CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].get_cycles(case_t);
			float time_temp = time_management -> time_unit_config(
				CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].get_cycles(case_t) / time_management -> sys_clk -> cur_freq
			); 
			time_management -> update_cur_time(time_temp + sys_clk -> cur_time);
			power_eval();
			cout << endl << time_management -> sys_clk -> cur_time << " us\t\t";
			*/
#ifdef DVFS_EN  // Before crossing to next basic block, checking if current basic block is a checkpoint
			if(dvfs_en == (char) DVFS_ENABLE) {
				// Invoking the operation of B-type checkpoint
				if(CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].B_checkpoint_en != 0x7FFFFFFF) {
					B_Intra_task_checkpoint(
						exe_path[cur_case_id][cur_block_index],    // Cast current Basic Block ID 
						exe_path[cur_case_id][cur_block_index + 1] // Cast its successive Basic Block ID according to the indicated execution path case
					);		
				}
				// Invoking the operation of L-type checkpoint
				else if(CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].L_checkpoint_en[0] != 0x7FFFFFFF) { cout << "LLLLLLLL" << endl;
					L_Intra_task_checkpoint(
						exe_path[cur_case_id][cur_block_index],    // Cast current Basic Block ID
						exe_path[cur_case_id][cur_block_index + 1] // Cast its successive Basic Block ID according ot the indicated execution path case
					);
				}			
				else if(CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].P_checkpoint_en != 0x7FFFFFFF) { 
					P_Intra_task_checkpoint(
						exe_path[cur_case_id][cur_block_index],    // Cast current Basic Block ID 
						exe_path[cur_case_id][cur_block_index + 1] // Cast its successive Basic Block ID according to the indicated execution path case
					);		
				}
				else {
				}	
			}	
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
			rem_cycles_temp -= CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].get_cycles(case_t);
			cur_block_index += 1;			
			if(exe_path[cur_case_id][cur_block_index] == 0x7FFFFFFF) {
			// The execution time for running our remaining execution cycles is less than one period 
			// of interrupt timer				
				sub_block_cycles -= rem_cycles_temp;  				
				break;
			}
			else executed_cycles = rem_cycles_temp; 		
		}
	}
	else if(rem_cycles_temp == 0) { // Remaining workload of current basic block takes same time as one period of interrupt timer
		executed_cycles = 0; // Reset the accumulation for next basic block
#ifdef DVFS_EN 
			if(dvfs_en == (char) DVFS_ENABLE) {
				// Invoking the operation of B-type checkpoint
				if(CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].B_checkpoint_en != 0x7FFFFFFF) {
					B_Intra_task_checkpoint(
						exe_path[cur_case_id][cur_block_index],    // Cast current Basic Block ID 
						exe_path[cur_case_id][cur_block_index + 1] // Cast its successive Basic Block ID according to the indicated execution path case
					);		
				}
				// Invoking the operation of L-type checkpoint
				else if(CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].L_checkpoint_en[0] != 0x7FFFFFFF) { cout << "LLLLLLLLLLLL" << endl;
					L_Intra_task_checkpoint(
						exe_path[cur_case_id][cur_block_index],    // Cast current Basic Block ID 
						exe_path[cur_case_id][cur_block_index + 1] // Cast its successive Basic Block ID according to the indicated execution path case
					);
				}			
				// Invoking the operation of P-type checkpoint
				else if(CFG_path[ exe_path[cur_case_id][cur_block_index] - 1 ].P_checkpoint_en != 0x7FFFFFFF) { 
					P_Intra_task_checkpoint(
						exe_path[cur_case_id][cur_block_index],    // Cast current Basic Block ID 
						exe_path[cur_case_id][cur_block_index + 1] // Cast its successive Basic Block ID according to the indicated execution path case
					);		
				}
				else {
				}	
			}	
#endif	
		cur_block_index += 1;
	}
	else { // Remaingin workload of current basic block takes a greater number of required exeuction time than one period of interrupt timer
		executed_cycles += sub_block_cycles; 
	}	
	
	// Returning the actual execution time within this period of interrupt period(including overhead)
	context_reg.act_cycles = sub_block_cycles; 
	context_reg.act_exe_time = ((float) sub_block_cycles / time_management -> sys_clk -> cur_freq);
	return context_reg;
	//power_eval();
	//global_param_eval(cycles_cnt);
}

/**
  * @brief For temporal test of function isr_driven_cfg() 
**/
void Src_CFG::dispatch_cfg(int &case_id, int case_t, float release_time_new, float start_time_new, float Deadline, char DVFS_en)
{
	float time_temp;
	dvfs_en = DVFS_en;
	isr_context_t context_reg;
	exe_speed_config();

//--------------------------------------------------------------------------------//
// Setting the release time, start time, absolute deadline and relative deadline
	// The release time of current(new) instance 
	release_time  = time_management -> time_unit_config(release_time_new);
	
	// The start time of current(new) instance
	start_time = time_management -> time_unit_config(start_time_new);  
	pre_eval_time = start_time;
	
	rel_dline = time_management -> time_unit_config(Deadline);
	abs_dline = release_time + rel_dline;

	time_management -> update_cur_time(start_time);
//--------------------------------------------------------------------------------//
#ifndef DEBUG
	cout << "==================================================================" << endl;
	printf("#	Release time: %.05f us, Start time: %.05f us\r\n", release_time, start_time);
	printf("#       Absolute Deadline: %.05f us\r\n", abs_dline);
	printf("#	max: %.02f MHz, min: %.02f MHz, Default speed: %.02f MHz\r\n", max_freq_t, min_freq_t, sys_clk -> cur_freq);
	printf("#	Jitter constraint: BCET + (WCET - BCET) * %.02f%\r\n", jitter_config.alpha * 100);
	cout << "==================================================================" << endl;
//	cout << "Start -> " << endl; cout << "current time: " << sys_clk -> cur_time << endl;
#endif

	executed_cycles = 0;
	cur_case_id = case_id;
	for(cur_block_index = 0; exe_path[case_id][cur_block_index] != 0x7FFFFFFF; ) {
#ifdef DEBUG
		cout << "Block_" << CFG_path[ exe_path[case_id][cur_block_index] - 1 ].get_index() << " -> ";
#endif
		context_reg = isr_driven_cfg((int) WORST, (char) DVFS_en);
		time_temp = time_management -> time_unit_config(
			context_reg.act_exe_time
		); 
		cycles_cnt += context_reg.act_cycles; 
		printf("\r\n#actual Execution cycles withing this period of interrupt timer: %d cycles\r\n", context_reg.act_cycles);
		printf("#Actual Execution time within this period of interrupt timer: %.05f us\r\n", time_temp);
		time_management -> update_cur_time(time_temp + sys_clk -> cur_time);
		printf("Current Time: %.05f us\r\n", time_management -> sys_clk -> cur_time);
		power_eval();

	}
#ifndef DEBUG
	cout << "End" << endl << endl;
#endif
	power_eval();
	global_param_eval();
	completion_config();
}

