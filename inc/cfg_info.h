#ifndef __CFG_INFO_H
#define __CFG_INFO_H

#include <iostream>
#include <vector>
#include <cmath>
#include "jitter_info.h"
#include "main.h"
#include "timer.h"
#include "checkpoint_info.h"

typedef std::vector<char> char_str;
typedef std::vector<int> array_int_element;
typedef std::vector<int> ExePath_case;
typedef std::vector<ExePath_case> ExePath_set;

class Basic_block {
	private:
		int block_index; 
		int execution_cycles[3]; // index_0 : WCEC, index_1 : ACEC, index_3 : BCEC

	public:
		std::vector<int> succ;
		//unsigned char loop_exit;
		int get_index(void);
		int get_cycles(int case_t);
		int get_succ(int succ_index);
		int B_checkpoint_en; // For normally branch instruction
		int L_checkpoint_en[2]; // For loop nest, first element means the index of loop nest; 
					// Second element means the address of minining table
		int P_checkpoint_en; // For labelling P-type checkpoint (Lookahead checkpoint)
		Basic_block(int curr_index, std::vector<int> &succ_index, std::vector<int> &cycles);
		~Basic_block(void);
};	

typedef struct isr_context{
	float act_exe_time;
	int act_cycles;
} isr_context_t;

class Src_CFG {
	private:
		std::vector<char_str> task_id;
		int TskID;
		char_str task_id_temp;
		std::vector<char_str> wcec, acec, bcec;
		char_str wcec_temp, acec_temp, bcec_temp;

		std::vector<array_int_element> task_succ;
		std::vector<int> succ;
		int succ_int_temp;
	
	public:
		std::vector<Basic_block> CFG_path;
		int execution_cycles[3]; // index_0 : WCEC, index_1 : ACEC, index_3 : BCEC
		float max_freq_t;
		float min_freq_t;
		float default_freq_t;
		char dvfs_en;

// Multitask scheduling information
		float response_time;
		float max_response;  // For evaluating the finish time jitter
		float min_response;  // For evaluating the finish time jitter
		double response_acc; // For evaluating the finish time jitter 
		double exe_acc;      // For evaluating the finish time jitter 	
		Src_CFG *next_task;
		Src_CFG *pre_task;
		ExePath_set exe_path;
		
		int cycle_acc;
		std::vector<float> exe_case;
		std::vector<float> response_case;
		float response_SampleVariance;
		float RFJ, AFJ; // Relative finishing time jitter; Absolute finishing time jitter
		float exe_var;  // Recording variation on actual execution time
		float tar_diff; // The difference between actual execution time and target execution time
		jitter_constraint jitter_config;
		std::vector<B_mining_table_t> B_mining_table;
		std::vector<L_mining_table_t> L_mining_table;
		std::vector<P_mining_table_t> P_mining_table;

	//public:
		void traverse_spec_path(int case_id, int case_t, float releast_time_new, float start_time_new, float Deadline, char DVFS_en);
		float get_cur_speed(void);

		// Intra-task DVFS attributes
		void checkpoints_placement(checkpoints_label *&checkpoint_label_temp);
		void mining_table_gen(void);
		void exe_cycle_tracing(exeTime_info WCET_INFO, RWCEC_Trace_in *cycle_in_temp, checkpoint_num *checkpointNum_temp);
		
		// Checkpoint Operation
		void B_Intra_task_checkpoint(int cur_block_index, int succ_block_index);
		void L_Intra_task_checkpoint(int cur_block_index, int succ_block_index);
		void P_Intra_task_checkpoint(int cur_block_index, int succ_block_index);
		
		float discrete_handle(float new_freq, int rwcec, float local_deadline);
		void checkpoint_operation(int block_index, int case_t);
		void exe_speed_config(void); // For determining all DVFS availability firstly
		void jitter_init(void); // Configuration of jitter constraints
		void exe_speed_scaling(float new_speed);
		void global_param_init(void);
		void constraint_update(void);
		void power_init(void);
		void power_eval(void);

		// The functions for final evaluation
		void global_param_eval(void);
		void output_result(char *case_msg);
		int cycles_cnt;
		

		//Configuration of Time Management 
		sys_clk_t *sys_clk;
		Time_Management *time_management; // Designate which time-management rules this task
		void timer_config(Time_Management *&timer);		

		// Test Case
		void pattern_init(ExePath_set test_case);
		int **P_loop_LaIteration;

		// Interrupt Timer and Preemption
		// Context register for interrupt timer and preemption
		isr_context_t isr_driven_cfg(int case_t, char DVFS_en);
		void dispatch_cfg(int &case_id, int case_t, float release_time_new, float start_time_new, float Deadline, char DVFS_en);
		int rem_wcec;		
		int executed_cycles;
		int cur_block_cycles;
		int cur_block_index;
		int cur_case_id;
		isr_context_t context_reg;

		// target control flow information
		Src_CFG(
			char *file_name, 
			Time_Management *timer, 
			checkpoints_label *checkpoint_label_temp, 
			RWCEC_Trace_in *cycle_in_temp,
			checkpoint_num *checkpointNum_temp,
			exeTime_info WCET_INFO,
			int TskID_in 
		);
		~Src_CFG(void);

		checkpoints_label *checkpointLabel;
		RWCEC_Trace_in *cycle_trace_in;
		checkpoint_num *checkpointNum;
	
	/*Since Lookahead Actual Loop iteration have been declared, so this two variables maybe no need*/	
	/*Note*/	std::vector< std::vector<int> > L_loop_iteration; // The counter of L-type iteration
	/*Note*/	std::vector<int> P_loop_iteration; // The counter of P-type iteration
		std::vector<int> L_loop_exit;

// Multitask scheduling information
		void completion_config(void);
		float start_time;
		float release_time;
		/*Perhaps no need*/char  state; // READY, WAIT, IDLE, RUN, TERMINATE
		float abs_dline; // Absolute Deadline
		float rel_dline; // Relative Deadline
		int dline_miss;
		float wcet;
		float wcrt; 
		float bcet;
		bool completion_flag; // True: just completed; False: haven't completed or arrived yet	
		/*Perhaps no need*/float period;
		/*Perhaps no need*/char  prt; // Task Priority

// Energy/Power Evaluation parameters
		float energy_acc;
		float pre_eval_time;
};

/*
Sample Variance
(s^2) = sum from {i = 1} to {n} { [(x_i - x')^2] / (n - 1) }
*/
static float sample_variance(vector<float> &a) 
{
	unsigned int i; float acc = 0.0, acc_1 = 0.0; 

	for(i = 0; i < a.size(); i++) acc += a[i]; 
	acc = acc / a.size(); 
	for(i = 0; i < a.size(); i++) acc_1 += ((a[i] - acc) * (a[i] - acc));
	acc_1 = acc_1 / (a.size() - 1);

	return acc_1; 
}

#endif // __CFG_INFO_H
