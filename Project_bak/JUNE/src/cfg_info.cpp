#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../inc/cfg_info.h"
#include "../inc/dvfs_info.h"
#include "../inc/main.h"

using namespace std;

extern float in_alpha;
extern float in_default_speed;
extern int sim_cnt;
extern int alpha_global;
extern int sys_mode;
extern double energy_ref;
float ISR_TIME_SLICE;

int Basic_block::get_cycles(int case_t)
{
	return execution_cycles[(int) (case_t - 1)];
}

int Basic_block::get_succ(int succ_index)
{
	return succ[succ_index];
}

int Basic_block::get_index(void)
{
	return block_index;
}

Basic_block::Basic_block(int curr_index, vector<int> &succ_index, vector<int> &cycles)
{
	int SuccNum = 0;
	int i;
	
	SuccNum = succ_index.size(); 

#ifdef DEBUG	
	cout << "SuccNum = " << SuccNum << endl;
#endif 
	for(i = 0; i < SuccNum; i++) succ.push_back(succ_index[i]);

	execution_cycles[0] = cycles[0];
	execution_cycles[1] = cycles[1];
	execution_cycles[2] = cycles[2];

	block_index = curr_index;
	B_checkpoint_en = 0x7FFFFFFF; // Disable itself as checkpoint 
	L_checkpoint_en[0] = 0x7FFFFFFF;
	L_checkpoint_en[1] = 0x7FFFFFFF;
#ifdef DEBUG	
	cout << "Block_" << block_index << ", ";
	cout << "WCEC: " << execution_cycles[0] << " ACEC: " << execution_cycles[1] << " BCEC: " << execution_cycles[2] << endl;
	cout << "Successors: ";
	for(i = 0; i < SuccNum; i++)
		cout << succ[i] << " ";
	cout << endl << endl;
#endif
}

Basic_block::~Basic_block(void)
{

}

Src_CFG::Src_CFG(
	char *file_name, 
	Time_Management *&timer, 
	checkpoints_t &checkpoints_temp, 
	RWCEC_Trace_in *cycle_trace_temp,
	int *WCET_INFO, 
	vector< vector<int> > test_case
)
{
	FILE *fp;
	int c, i, j, k;
	unsigned char fsm_state = (unsigned char) ATTRIBUTES_ITEM;
	unsigned char fsm_state_internal = (unsigned char) TASK_ID;
	vector<int> succ_conv_temp;
	int size_temp;
	succ_int_temp = 0;
	fp = fopen(file_name, "r");

	while(1) {
		c = fgetc(fp); 
		if( feof(fp) ) break;
		else  {
			if(c == 0x0A || c == ' ') { while( c == 0x0A || c == ' ' ) c = fgetc(fp);  }
		//	printf("c = %c state:0x%02X 0x%02X\r\n", c, (unsigned char) fsm_state, (unsigned char) fsm_state_internal);	
			if( c == '{' || c == '}' ) {		
				switch(c)
				{
					case '{': // Identifying 1) Task Attributes or 2) Its successors list
						fsm_state = (fsm_state == (unsigned char) ATTRIBUTES_ITEM) ? (unsigned char) TASK_ATTRIBUTES : 
							    (fsm_state == (unsigned char) SUCCESSORS_ITEM) ? (unsigned char) SUCCESSORS      :
											                             fsm_state;
						break;
				
					case '}': 
						c = fgetc(fp);
						if(c == ',') 
							fsm_state = (unsigned char) SUCCESSORS_ITEM;
						else if(c == ';') {
							fsm_state = (unsigned char) ATTRIBUTES_ITEM;
							fsm_state_internal = (unsigned char) TASK_ID;
							wcec.push_back(wcec_temp); wcec_temp.clear();
							acec.push_back(acec_temp); acec_temp.clear();
							bcec.push_back(bcec_temp); bcec_temp.clear();
							
							// After last element, clearing is still needed
							succ.push_back(succ_int_temp); succ_int_temp = 0; 
							
							task_succ.push_back(succ); succ.clear();
							cout << endl;
						}
						else 
							cout << "Wrong Format, expecting either\",\" or \";\"" << endl;
						break;
	
					default:
						break;
				}
			}
			else {
				if(fsm_state == (unsigned char) TASK_ATTRIBUTES && fsm_state_internal == (unsigned char) TASK_ID) {
					if(c != ',') task_id_temp.push_back(c);
					else { task_id.push_back(task_id_temp); task_id_temp.clear(); fsm_state_internal += 0x01; } 
				}
				else if(fsm_state == (unsigned char) TASK_ATTRIBUTES && fsm_state_internal != (unsigned char) TASK_ID) {
					if     (fsm_state_internal == (unsigned char) WCEC && c != ',') wcec_temp.push_back(c);
					else if(fsm_state_internal == (unsigned char) ACEC && c != ',') acec_temp.push_back(c);
					else if(fsm_state_internal == (unsigned char) BCEC && c != '}') bcec_temp.push_back(c);
					else fsm_state_internal += 0x01; 
											   
				}
				else { // fsm_state = SUCCESSORS
					if(c != ',') {
						succ_int_temp = succ_int_temp * 10;
						succ_int_temp = (c != '#') ? succ_int_temp + ( c - 48 ) : 0;		
					}
					else { // c = ','
						succ.push_back(succ_int_temp);
						succ_int_temp = 0;
					}
				}
			}
		}
	}
	fclose(fp);

	for(i = 0; i < task_id.size(); i++) {
		int id_temp = 0; vector<int> cycles_temp;

		for(j=0;j<task_id[i].size();j++) {
			id_temp *= 10;
			id_temp += ( task_id[i][j] - 48 );
		}

		for(j=0;j<task_succ[i].size();j++) succ_conv_temp.push_back(task_succ[i][j]);//succ_conv_temp[j] = task_succ[i][j];

		cycles_temp.push_back((int) 0); cycles_temp.push_back( (int) 0 ); cycles_temp.push_back( (int) 0 );
		for(j=0;j<wcec[i].size();j++) {	
			cycles_temp[0] *= 10;
			cycles_temp[0] += ( wcec[i][j] - 48 );
		}
		
		for(j=0;j<acec[i].size();j++) {	
			cycles_temp[1] *= 10;
			cycles_temp[1] += ( acec[i][j] - 48 );
		}
		
		for(j=0;j<bcec[i].size();j++) {	
			cycles_temp[2] *= 10;
			cycles_temp[2] += ( bcec[i][j] - 48 );
		}
		
		CFG_path.push_back( 
					Basic_block( 
						(int) id_temp, // Task id
						      succ_conv_temp, // Enumeration of all successors
						      cycles_temp 
					) 
		);
		vector<int>().swap(cycles_temp); vector<int>().swap(succ_conv_temp);
	}

	// According to the input file
	exe_cycle_tracing(WCET_INFO, cycle_trace_temp);
	
	// Checkpoints insertion and generating its corresponding mining table
	checkpoints_placement(checkpoints_temp); mining_table_gen();
	
	// Designating the system-clock/timer source
	timer_config(timer);
	
	global_param_init();

	pattern_init(test_case);
	
	vector<int>().swap(succ_conv_temp);
}

Src_CFG::~Src_CFG(void)
{
}

void Src_CFG::timer_config(Time_Management *&timer)
{
	time_management = timer;
	sys_clk = timer -> sys_clk;
}

void Src_CFG::pattern_init(vector< vector<int> > test_case)
{
	exe_path = test_case;
}

void Src_CFG::global_param_init(void)
{
	// Setting the DVFS-available specification
	exe_speed_config();
	
	// Setting the Jitter constraints
	jitter_init(); 
	
	// Initialising some global parameters
	exe_var = 0.0;
	response_time = 0.0;
	max_response = 0.0;
	min_response = 0x7FFFFFFF;
	AFJ = 0.0;
	RFJ = 0.0;
	response_acc = 0.0;
	exe_acc = 0.0;
	cycle_acc = 0;
	wcet = (float) (execution_cycles[WORST - 1] / default_freq_t);//min_freq_t); 
	bcet = (float) (execution_cycles[BEST - 1] / max_freq_t);
	dline_miss = 0;
	completion_flag = false;
	rem_wcec = 0;
	cycles_cnt = 0;

	// Initialising the context registe
	ISR_TIME_SLICE = INST_UNIT / default_freq_t;
	executed_cycles = 0;
	cur_block_cycles = 0;
	cur_case_id = 0;

	// Initialising energy/power evaluation parameters
	power_init();
}

void Src_CFG::global_param_eval(void)
{
	float Act_CET, response_pre, response_cur, response_diff;
 
	dline_miss = (sys_clk -> cur_time > abs_dline) ? dline_miss + 1 : dline_miss;
	if(dline_miss != 0) {
		cout << endl << endl << endl;
		cout << "Deadline miss" << endl;
		while(1);
	}
	Act_CET = (float) (sys_clk -> cur_time - start_time);
	cycle_acc += cycles_cnt;
	exe_acc += Act_CET;
	exe_case.push_back(Act_CET);
	response_pre = response_time;
	response_cur = sys_clk -> cur_time - release_time;
	response_time = response_cur;
	response_diff = (response_pre == 0) ? 0: 
			(response_cur >= response_pre) ? (response_cur - response_pre) : (response_pre - response_cur);
	response_acc += response_cur;
	response_case.push_back(response_cur);
	response_SampleVariance = sample_variance(response_case); 

	tar_diff = jitter_config.exe_time_target - Act_CET;
	tar_diff = (tar_diff / jitter_config.exe_time_target) * 100;
	exe_var  = (((exe_acc / exe_case.size()) - bcet) / jitter_config.exe_jitter_bound) * 100;
	max_response = (max_response < response_cur) ? response_cur : max_response;
	min_response = (min_response > response_cur) ? response_cur : min_response;
	AFJ = max_response - min_response;
	RFJ = (RFJ < response_diff) ? response_diff : RFJ;
#ifndef DEBUG
	cout << "Actual Execution Cycles: " << cycles_cnt << " cycles" << endl;
	printf("Relative Deadline: %.05f us\r\n", rel_dline);
	printf("Worst Case Execution Time: %.05f us\r\n", wcet);
	printf("Best Case Execution Time: %.05f us\r\n", bcet);
	printf("Target Execution Time: %.05f us(%.02f%%)\r\n", jitter_config.exe_time_target, jitter_config.alpha * 100);
	printf("Actual Execution Time: %.05f us(%.02f%%)\r\n", Act_CET, ((Act_CET - bcet)  / jitter_config.exe_jitter_bound) * 100);  
	printf("Response Time: %.05f us.\r\n", response_cur);
	printf("Absolute Deadline: %.05f us\r\n", abs_dline);
	printf("Completion time: %.05f us\r\n", sys_clk -> cur_time);
	printf("The number of Deadline miss: %d\r\n", dline_miss);
	//printf("The difference between actual and target execution time: %.02f\% \r\n", tar_diff);
	printf("The total exeuction cycles: %d cycles\r\n", cycle_acc);
//	printf("The variation on execution time: %.02f% \r\n", exe_var);
	printf("Sample Standard Deviation: %.05f us(%.02f%%)\r\n", sqrt(response_SampleVariance), (sqrt(response_SampleVariance) / jitter_config.exe_jitter_bound) * 100);
	printf("Relative finishing time jitter(RFJ): %.05f us.\r\n", RFJ);
	printf("Absolute finishing time jitter(AFJ): %.05f us.\r\n", AFJ);
	printf("Energy consumption: %.05f uJ(%.02f%% Energy-Saving)\r\n", energy_acc * 1000, ((energy_ref - energy_acc) / energy_ref) * 100);
#endif	
		if(dline_miss != 0) {cout << "# Missing Deadline!" << endl; while(1);}
		//jitter_config.exe_time_target = exe_acc / exe_case.size();
		//jitter_config.alpha = exe_var / 100; 	
		//jitter_init(); // Updating the jitter constraint if it is needed
}

void Src_CFG::output_result(char* case_msg) {
	char ExeVar_msg[152];
	
	sprintf(ExeVar_msg, "echo \"%sDefault_Freq: %.02fMHz -> Std_dev(Response):%.02f\%, RFJ:%.02f(us), AFJ:%.02f(us), Response(avg):%.02f\%\" >> test_result%.02f_%.01f.txt", 
		case_msg,
		default_freq_t, 
	        (sqrt(response_SampleVariance) / jitter_config.exe_jitter_bound) * 100,
		RFJ,
		AFJ,
		exe_var,
		in_alpha,
		in_default_speed
	);
	system(ExeVar_msg); 
}

void Src_CFG::power_init(void)
{
	energy_acc    = 0.0;
	pre_eval_time = 0.0;
}

void Src_CFG::power_eval(void)
{
#ifdef PROCESSOR_AM335x
	int i;
	for(i = 0; sys_clk -> cur_freq != freq_vol[i][0]; i++);
	energy_acc += (sys_clk -> cur_time - pre_eval_time) * (MPU_POWER[i] / 1000000);
	pre_eval_time = sys_clk -> cur_time;
	
	energy_ref = (dvfs_en == (char) DVFS_DISABLE) ? energy_acc : energy_ref;
#endif
}

void Src_CFG::completion_config(void)
{
	completion_flag = true;
	L_loop_iteration.at(0) = 3 + 1;
	cycles_cnt = 0;
}

void Src_CFG::traverse_spec_path(int &case_id, int case_t, float release_time_new, float start_time_new, float Deadline, char DVFS_en)
{
	int cur_index, i;
	float time_temp;
	dvfs_en = DVFS_en;

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
#ifdef DEBUG
	cout << "==================================================================" << endl;
	printf("#	Release time: %.05f us, Start time: %.05f us\r\n", release_time, start_time);
	printf("#	max: %.02f MHz, min: %.02f MHz, Default speed: %.02f MHz\r\n", max_freq_t, min_freq_t, sys_clk -> cur_freq);
	printf("#	Jitter constraint: BCET + (WCET - BCET) * %.02f%\r\n", jitter_config.alpha * 100);
	cout << "==================================================================" << endl;
	cout << "Start -> " << endl; cout << "current time: " << sys_clk -> cur_time << endl;
#endif
	for(cur_index = 0; exe_path[case_id][cur_index] != 0x7FFFFFFF; cur_index++) {
#ifdef DEBUG
		cout << "Block_" << CFG_path[ exe_path[case_id][cur_index] - 1 ].get_index() << " -> ";
#endif
		cycles_cnt += CFG_path[ exe_path[case_id][cur_index] - 1 ].get_cycles(case_t);

		time_temp = time_management -> time_unit_config(
			CFG_path[ exe_path[case_id][cur_index] - 1 ].get_cycles(case_t) / time_management -> sys_clk -> cur_freq
		); 
		time_management -> update_cur_time(time_temp + sys_clk -> cur_time);
		power_eval();
#ifdef DVFS_EN 
	if(dvfs_en == (char) DVFS_ENABLE) {
		// Invoking the operation of B-type checkpoint
		if(CFG_path[ exe_path[case_id][cur_index] - 1 ].B_checkpoint_en != 0x7FFFFFFF) {
			B_Intra_task_checkpoint(exe_path[case_id][cur_index], exe_path[case_id][cur_index + 1]);		
		}
		// Invoking the operation of L-type checkpoint
		else if(CFG_path[ exe_path[case_id][cur_index] - 1 ].L_checkpoint_en[0] != 0x7FFFFFFF) {
			L_Intra_task_checkpoint(exe_path[case_id][cur_index], exe_path[case_id][cur_index + 1]);
		}			
	}	
#endif
	}
#ifdef DEBUG
	cout << "End" << endl << endl;
#endif
	power_eval();
	global_param_eval();
	completion_config();
}

void Src_CFG::exe_speed_config(void)
{
	// DVFS setting
	max_freq_t     = (float) MAX_speed;
	min_freq_t     = (float) MIN_speed;
	default_freq_t = (in_default_speed > max_freq_t) ? max_freq_t : (in_default_speed < min_freq_t) ? min_freq_t : in_default_speed;
	//default_freq_t = (default_freq_t != max_freq_t && default_freq_t != min_freq_t) ? discrete_handle(default_freq_t) : default_freq_t;
	time_management -> cur_freq_config(default_freq_t);
	
}
void Src_CFG::constraint_update(void)
{
	float bcet_limit, wcet_limit, middle_t;

	if(alpha_global == 1) jitter_config.alpha = in_alpha;
	else if(alpha_global == 2) jitter_config.alpha = exe_var / 100; 	
	else if(alpha_global == 3){
	bcet_limit = ((execution_cycles[BEST  - 1] / min_freq_t) - bcet) / jitter_config.exe_jitter_bound; 
	wcet_limit = ((execution_cycles[WORST - 1] / max_freq_t) - bcet) / jitter_config.exe_jitter_bound; 
	jitter_config.alpha = (in_alpha < bcet_limit) ? bcet_limit :
			      (in_alpha > wcet_limit) ? wcet_limit : in_alpha; 
	}
	else {
		bcet_limit = ((execution_cycles[BEST  - 1] / min_freq_t) - bcet) / jitter_config.exe_jitter_bound;
		wcet_limit = ((execution_cycles[WORST - 1] / max_freq_t) - bcet) / jitter_config.exe_jitter_bound;
		middle_t = (sys_mode == (int) H_RESP ) ? wcet_limit / 2 :
			   (sys_mode == (int) L_POWER) ? (((float) 1.0 - bcet_limit) / 2) + bcet_limit : 0;
		jitter_config.alpha = ((in_alpha > wcet_limit) && (sys_mode == (int) H_RESP)) ? middle_t :
				      ((in_alpha < bcet_limit) && (sys_mode == (int) L_POWER)) ? middle_t : in_alpha; 
	}
	
}
 
void Src_CFG::jitter_init(void)
{
	// Jitter constraint setting
	jitter_config.exe_jitter_bound = ((float) (execution_cycles[WORST - 1] / min_freq_t)) - ((float) (execution_cycles[BEST - 1] / max_freq_t));
	constraint_update();
	jitter_config.exe_time_target = (execution_cycles[BEST - 1] / max_freq_t) + jitter_config.exe_jitter_bound * jitter_config.alpha;	
}

void Src_CFG::exe_speed_scaling(float new_speed)
{
	time_management -> cur_freq_config(new_speed);
	ISR_TIME_SLICE = INST_UNIT / time_management -> sys_clk -> cur_freq;
#ifndef DEBUG
	printf("Current speed: %.02f MHz\r\n\r\n", time_management -> sys_clk -> cur_freq);
	printf("The period of interrupt timer has been changed to %.08f us\r\n", ISR_TIME_SLICE);	
#endif
}

float Src_CFG::get_cur_speed(void)
{
	return time_management -> sys_clk -> cur_freq;
}

void Src_CFG::B_Intra_task_checkpoint(int cur_block_index, int succ_block_index)
{
	float new_freq;
	float exe_time_target;
	float exe_time_expect; // A speculative execution time if keeping current execution speed withou any change
	float elapsed_time = time_management -> sys_clk -> cur_time - release_time; 
	float target_comparison;
	int rwcec; // Remaining worst-case execution cycles from current basic block

	if( succ_block_index == B_mining_table[ CFG_path[cur_block_index - 1].B_checkpoint_en].successors[0] ) {
		rwcec = B_mining_table[ CFG_path[cur_block_index - 1].B_checkpoint_en ].n_taken_rwcec;
		rem_wcec = rwcec;
#ifdef DEBUG
		cout << endl << "not taken" << endl;
#endif
	}
	else {
		rwcec = B_mining_table[ CFG_path[cur_block_index - 1].B_checkpoint_en ].taken_rwcec;
		rem_wcec = rwcec;
#ifdef DEBUG
		cout << endl << "taken" << endl;
#endif
	}
#ifdef DEBUG
	printf("rwcec = %d, ", rwcec); 
#endif
	exe_time_expect = elapsed_time + (rwcec / sys_clk -> cur_freq); 
	if(elapsed_time > jitter_config.exe_time_target) {
#ifdef DEBUG
		cout << endl << "Updating Jitter Constraint from " << exe_time_target << "us to " << exe_time_expect << "us" << endl;
#endif
		exe_time_target = exe_time_expect;
	}//
        else exe_time_target = jitter_config.exe_time_target;	
	//target_comparison = ((exe_time_target - exe_time_expect) < 0) ? (exe_time_target - exe_time_expect) * (-1) : (exe_time_target - exe_time_expect);
	target_comparison = exe_time_target - exe_time_expect;

if(alpha_global == 3) {
	if( (float) target_comparison != 0.0 ) {
#ifdef DEBUG
		printf("cur_block: %d, succ_block: %d\r\n", cur_block_index, succ_block_index);
		printf("Current time: %f us, ", sys_clk -> cur_time);
		printf("Elapsed time = %f us, exe_time_target = %f us, exe_time_expect = %f us, ", 
					elapsed_time, 
					exe_time_target, 
					exe_time_expect
		);
		printf("Difference: %.05f us\r\n", target_comparison);
#endif
		if((exe_time_target - elapsed_time) <= (abs_dline - sys_clk -> cur_time)) new_freq = rwcec / (exe_time_target - elapsed_time);
		else new_freq = rwcec / (abs_dline - sys_clk -> cur_time);
		//new_freq = ((float) target_comparison < 0) ? rwcec / (elapsed_time - exe_time_target) : rwcec / (exe_time_target - elapsed_time);
		//new_freq = ((float) target_comparison < 0) ? max_freq_t : rwcec / (exe_time_target - elapsed_time);
		new_freq = (new_freq > max_freq_t) ? max_freq_t : 
		           (new_freq < min_freq_t) ? min_freq_t : new_freq;
	}
	else 
		new_freq = sys_clk -> cur_freq;
}	
else if(alpha_global == 4) {
	//if(exe_time_target < exe_time_expect) {
#ifdef DEBUG
		printf("cur_block: %d, succ_block: %d\r\n", cur_block_index, succ_block_index);
		printf("Current time: %f us, ", sys_clk -> cur_time);
		printf("Elapsed time = %f us, exe_time_target = %f us, exe_time_expect = %f us, ", 
					elapsed_time, 
					exe_time_target, 
					exe_time_expect
		);
		//printf("Difference: %.05f us\r\n", target_comparison);
#endif
		new_freq = rwcec / (abs_dline - sys_clk -> cur_time);
		//new_freq = rwcec / (exe_time_target - elapsed_time);
		//new_freq = ((float) target_comparison < 0) ? rwcec / (elapsed_time - exe_time_target) : rwcec / (exe_time_target - elapsed_time);
		//new_freq = ((float) target_comparison < 0) ? max_freq_t : rwcec / (exe_time_target - elapsed_time);
		new_freq = (new_freq > max_freq_t) ? max_freq_t : 
		           (new_freq < min_freq_t) ? min_freq_t : new_freq;
	//}
	//else 
	//	new_freq = sys_clk -> cur_freq;
}	
#ifdef DISCRETE_DVFS
	if(new_freq != min_freq_t && new_freq != max_freq_t) new_freq = discrete_handle(new_freq, rwcec);
#endif
	exe_speed_scaling(new_freq);
}

void Src_CFG::L_Intra_task_checkpoint(int cur_block_index, int succ_block_index)
{
	float new_freq;
	float exe_time_target;
	float exe_time_expect; // A speculative execution time if keeping current execution speed withou any change
	float elapsed_time = sys_clk -> cur_time - release_time;
	float target_comparison;
	int rwcec; // Remaining worst-case execution cycles from current basic block
	int loop_index = CFG_path[cur_block_index - 1].L_checkpoint_en[0];
	int loop_addr  = CFG_path[cur_block_index - 1].L_checkpoint_en[1];
	
	if(cur_block_index == L_loop_exit[loop_index]) L_loop_iteration.at(loop_index) = L_loop_iteration[loop_index] - 1;
	if( succ_block_index == L_mining_table[loop_index][loop_addr].successors[0] ) {
		int temp = ((L_loop_iteration[loop_index] - 1) == -1) ? 0 : (L_loop_iteration[loop_index] - 1);
		rwcec = L_mining_table[loop_index][loop_addr].n_taken_rwcec[temp]; 
		rem_wcec = rwcec;
#ifdef DEBUG
		cout << endl << "not taken" << endl; 
#endif
	}
	else {
		rwcec = L_mining_table[loop_index][loop_addr].taken_rwcec[L_loop_iteration[loop_index] - 1]; 
		rem_wcec = rwcec;
#ifdef DEBUG
		cout << endl << "taken" << endl; 
#endif
	}
#ifdef DEBUG
	printf("rwcec = %d, ", rwcec);
#endif
	exe_time_expect = elapsed_time + (rwcec / sys_clk -> cur_freq); 
	if(elapsed_time > jitter_config.exe_time_target) {
#ifdef DEBUG
		cout << endl << "Updating Jitter Constraint from " << exe_time_target << "us to " << exe_time_expect << "us" << endl;
#endif
		exe_time_target = exe_time_expect;
	}
        else exe_time_target = jitter_config.exe_time_target;	
	//target_comparison = ((exe_time_target - exe_time_expect) < 0) ? (exe_time_target - exe_time_expect) * (-1) : (exe_time_target - exe_time_expect);
	target_comparison = exe_time_target - exe_time_expect;
if(alpha_global == 3) {
	if( (float) target_comparison != 0.0 ) {
#ifdef DEBUG
		printf("cur_block: %d, succ_block: %d\r\n", cur_block_index, succ_block_index);
		printf("Current time: %.05f us, ", sys_clk -> cur_time);
		printf("Elapsed time = %.05f us, exe_time_target = %.05f us, exe_time_expect = %.05f us, ", 
					elapsed_time, 
					exe_time_target, 
					exe_time_expect
		);
		printf("Difference: %.05f us\r\n", target_comparison);
#endif
		if((exe_time_target - elapsed_time) <= (abs_dline - sys_clk -> cur_time)) new_freq = rwcec / (exe_time_target - elapsed_time);
		else new_freq = rwcec / (abs_dline - sys_clk -> cur_time);
		//new_freq = ((float) target_comparison < 0) ? rwcec / (elapsed_time - exe_time_target) : rwcec / (exe_time_target - elapsed_time);
		//new_freq = ((float) target_comparison < 0) ? max_freq_t : rwcec / (exe_time_target - elapsed_time);
		new_freq = (new_freq > max_freq_t) ? max_freq_t : 
		           (new_freq < min_freq_t) ? min_freq_t : new_freq;
	}
	else 
		new_freq = sys_clk -> cur_freq;
}
else if(alpha_global == 4) {
	//if(exe_time_target < exe_time_expect) {
#ifdef DEBUG
		printf("cur_block: %d, succ_block: %d\r\n", cur_block_index, succ_block_index);
		printf("Current time: %.05f us, ", sys_clk -> cur_time);
		printf("Elapsed time = %.05f us, exe_time_target = %.05f us, exe_time_expect = %.05f us, ", 
				elapsed_time, 
				exe_time_target, 
				exe_time_expect
		);
		//printf("Difference: %.05f us\r\n", target_comparison);
#endif
		new_freq = rwcec / (abs_dline - sys_clk -> cur_time);
		//new_freq = rwcec / (exe_time_target - elapsed_time);
		//new_freq = ((float) target_comparison < 0) ? rwcec / (elapsed_time - exe_time_target) : rwcec / (exe_time_target - elapsed_time);
		//new_freq = ((float) target_comparison < 0) ? max_freq_t : rwcec / (exe_time_target - elapsed_time);
		new_freq = (new_freq > max_freq_t) ? max_freq_t : 
		           (new_freq < min_freq_t) ? min_freq_t : new_freq;
	//}
	//else 
	//	new_freq = sys_clk -> cur_freq;
}
#ifdef DISCRETE_DVFS
	if(new_freq != min_freq_t && new_freq != max_freq_t) new_freq = discrete_handle(new_freq, rwcec);
#endif
	exe_speed_scaling(new_freq);
}

float Src_CFG::discrete_handle(float new_freq, int rwcec)
{
	int i;
	float max_diff, min_diff;
	
	for(i = 0; new_freq >= freq_vol[i+1][0]; i++);
#ifdef DEBUG
	printf("Discrete Bound: %.02f MHz, %.02f MHz, %.02f MHz\r\n", freq_vol[i][0], new_freq, freq_vol[i+1][0]);
#endif
	if(alpha_global != 4) {
		min_diff = new_freq - freq_vol[i][0];
		max_diff = freq_vol[i+1][0] - new_freq;
		return (min_diff < max_diff && (sys_clk -> cur_time + rwcec / freq_vol[i][0]) <= abs_dline) ? freq_vol[i][0] : freq_vol[i+1][0];	
	}
	else return freq_vol[i+1][0]; // In the case of Low Power Mode
}

/**
Execution traces obtained via a cycle-level simulation. 
The results will be given by existing WCET toolsets.
**/
void Src_CFG::exe_cycle_tracing(int *WCET_INFO, RWCEC_Trace_in *cycle_trace_temp)
{
/*	execution_cycles[WORST - 1]   = 425;
	execution_cycles[AVERAGE - 1] = 80; 
	execution_cycles[BEST - 1]    = 30;
*/
	execution_cycles[WORST - 1]   = WCET_INFO[0];//605;
	execution_cycles[AVERAGE - 1] = WCET_INFO[1];//80; 
	execution_cycles[BEST - 1]    = WCET_INFO[2];//75;

	cycle_trace_in = cycle_trace_temp;

	// According to the Input file
	L_loop_cnt = 1; // There is only one loop nest existing in this CFG
	loop_bound.push_back(3);
}

void Src_CFG::mining_table_gen(void)
{
	int i, j, index_temp, B_checkpoints_cnt, L_checkpoints_cnt;

// Creating the correspoinding miniing table in order to record the remaining worst-case execution cycles at each basic block (especially every branch)
// For B-type checkpoints
	B_checkpoints_cnt = B_checkpoints.size();
	for( i = 0; i < B_checkpoints_cnt; i++ ) 
		B_mining_table.push_back( (B_mining_table_t) {
							cycle_trace_in -> B_RWCEC_t[i][NOT_TAKEN], 
							cycle_trace_in -> B_RWCEC_t[i][TAKEN], 
							cycle_trace_in -> B_RWCEC_t[i][0], 
							cycle_trace_in -> B_RWCEC_t[i][2]
						} 
		);
	
#ifdef DEBUG
	cout << "B-Type Mining Table" << endl;
	cout << "------------------------------------------------" << endl;
	cout << "|WCEC(cycle): " << execution_cycles[WORST - 1] << "\t\t\t\t|" << endl;
	cout << " -----------------------------------------------" << endl;
	cout << "| Address\t|" << "\t\tRWCEC(cycle)\t|" << endl;
	cout << "\t\t -------------------------------" << endl;
	cout << "|\t\t|\t" << "n_taken\t|\ttaken\t|" << endl;
	cout << " -----------------------------------------------" << endl;
	for( i = 0; i < B_checkpoints_cnt; i++ ) {
		cout << "| Branch_" 
		     << i << "\t|\t" 
		     << B_mining_table[i].n_taken_rwcec 
		     << "(" << B_mining_table[i].successors[0] 
		     << ")\t|\t" 
		     << B_mining_table[i].taken_rwcec 
		     << "(" << B_mining_table[i].successors[1]
	             << ")\t|" << endl; 
		cout << " -----------------------------------------------" << endl;	
	}
	cout << endl;
#endif 	

// For L-type checkpoints
	for(index_temp = 0; index_temp < L_loop_cnt; index_temp++) { // The #th Loop-nest
		vector<L_mining_table_t> loop_t;
		L_checkpoints_cnt = L_checkpoints[index_temp].size();
		for(i = 0; i < L_checkpoints_cnt; i++) { // The checkpoints inside #th Loop-nest
			vector<int> n_taken_t, taken_t;
			for(j = 0; j < loop_bound[index_temp]; j++) { // The #th iteration of each checkpoint inside each Loop-nest
				n_taken_t.push_back(cycle_trace_in -> L_RWCEC_t[index_temp][i][NOT_TAKEN * (j + 1)]);
				taken_t.push_back(cycle_trace_in -> L_RWCEC_t[index_temp][i][(loop_bound[index_temp] + 2) + j]);
			}
			loop_t.push_back( 
				{
					n_taken_t,
					taken_t,
					cycle_trace_in -> L_RWCEC_t[index_temp][i][0], // Not Taken
					cycle_trace_in -> L_RWCEC_t[index_temp][i][loop_bound[index_temp] + 1], // Taken
				} 
			);
			vector<int>().swap(n_taken_t); vector<int>().swap(taken_t);
		}
		L_mining_table.push_back(loop_t);
		vector<L_mining_table_t>().swap(loop_t);		
	}

#ifdef DEBUG
for(index_temp = 0; index_temp < L_loop_cnt; index_temp++) {	
	L_checkpoints_cnt = L_checkpoints[index_temp].size(); 
	cout << "L-Type Mining Table_" << index_temp << endl;
	cout << "------------------------------------------------" << endl;
	cout << "Loop Bound(iteration): " << loop_bound[index_temp] << endl;
	for( i = 0; i < L_checkpoints_cnt; i++ ) {
		cout << "Branch_" << i << ":" << endl; 
		for(j = loop_bound[index_temp] - 1; j >= 0; j--) {
			cout << "n_taken_" << (j + 1) << ": " 
		             << L_mining_table[index_temp][i].n_taken_rwcec[j] << "(" << L_mining_table[index_temp][i].successors[0] <<")\t";
			cout << "taken_" << (j + 1) << ": " 
		             << L_mining_table[index_temp][i].taken_rwcec[j] << "(" << L_mining_table[index_temp][i].successors[1] << ")" << endl;
		}
		cout << endl;
	}
	cout << "------------------------------------------------" << endl;
	cout << endl << endl;
}
#endif 	

// For P-type checkpoints
/*	for(index_temp = 0; index_temp < L_loop_cnt; index_temp++) { // The #th yLoop-nest
		vector<L_mining_table_t> loop_t;
		L_checkpoints_cnt = L_checkpoints[index_temp].size();
		for(i = 0; i < L_checkpoints_cnt; i++) { // The checkpoints inside #th Loop-nest
			vector<int> n_taken_t, taken_t;
			for(j = 0; j < loop_bound[index_temp]; j++) { // The #th iteration of each checkpoint inside each Loop-nest
				n_taken_t.push_back(L_RWCEC_t[index_temp][i][NOT_TAKEN * (j + 1)]);
				taken_t.push_back(L_RWCEC_t[index_temp][i][(loop_bound[index_temp] + 2) + j]);
			}
			loop_t.push_back( 
				{
					n_taken_t,
					taken_t,
					L_RWCEC_t[index_temp][i][0], // Not Taken
					L_RWCEC_t[index_temp][i][loop_bound[index_temp] + 1], // Taken
				} 
			);
			vector<int>().swap(n_taken_t); vector<int>().swap(taken_t);
		}
		L_mining_table.push_back(loop_t);
		vector<L_mining_table_t>().swap(loop_t);		
	}

for(index_temp = 0; index_temp < L_loop_cnt; index_temp++) {	
	L_checkpoints_cnt = L_checkpoints[index_temp].size(); 
	cout << "L-Type Mining Table_" << index_temp << endl;
	cout << "------------------------------------------------" << endl;
	cout << "Loop Bound(iteration): " << loop_bound[index_temp] << endl;
	for( i = 0; i < L_checkpoints_cnt; i++ ) {
		cout << "Branch_" << i << ":" << endl; 
		for(j = loop_bound[index_temp] - 1; j >= 0; j--) {
			cout << "n_taken_" << (j + 1) << ": " 
		             << L_mining_table[index_temp][i].n_taken_rwcec[j] << "(" << L_mining_table[index_temp][i].successors[0] <<")\t";
			cout << "taken_" << (j + 1) << ": " 
		             << L_mining_table[index_temp][i].taken_rwcec[j] << "(" << L_mining_table[index_temp][i].successors[1] << ")" << endl;
		}
		cout << endl;
	}
	cout << "------------------------------------------------" << endl;
	cout << endl << endl;
}
*/
}

void Src_CFG::checkpoints_placement(checkpoints_t &checkpoints_temp)
{	
	int B_cnt, L_cnt, index_temp, i;
	
	B_cnt = checkpoints_temp.B_checkpoints.size();
	
	for( index_temp = 0; index_temp < B_cnt; index_temp++ ) {
		B_checkpoints.push_back( checkpoints_temp.B_checkpoints[index_temp] );
		CFG_path[ checkpoints_temp.B_checkpoints[index_temp] - 1 ].B_checkpoint_en = index_temp; // Enable the corrsponding Basic Block as a checkpoint
	} 
	for( index_temp = 0; index_temp < L_loop_cnt; index_temp++ ) {
		L_cnt = checkpoints_temp.L_checkpoints[index_temp].size();
		L_checkpoints.push_back( checkpoints_temp.L_checkpoints[index_temp] );
		for(i = 0; i < L_cnt; i++) {
			CFG_path[ checkpoints_temp.L_checkpoints[index_temp][i] - 1 ].L_checkpoint_en[0] = index_temp;
			CFG_path[ checkpoints_temp.L_checkpoints[index_temp][i] - 1 ].L_checkpoint_en[1] = i;
		}
	 	L_loop_iteration.push_back(checkpoints_temp.L_loop_iteration[index_temp]);
		L_loop_exit.push_back(checkpoints_temp.L_checkpoints[index_temp].front()); 
	}
}
