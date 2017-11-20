void Src_CFG::B_Intra_task_checkpoint(int cur_block_index, int succ_block_index)
{
	float new_freq;
	float rep_time_target;
	float rep_time_expect; // A speculative response time if keeping current execution speed withou any change
	float elapsed_time; 
	float target_comparison, rem_time, executed_time;
	int rwcec; // Remaining worst-case execution cycles from current basic block
	int branch_addr = CFG_path[cur_block_index - 1].B_checkpoint_en;
//===============================================================================================================//
// Look up the remaining worst-case exeuction cycles (RWCEC) from B-type mining table
	// Identify the actual branch
	if( succ_block_index == B_mining_table[branch_addr].successors[0] ) {
		rwcec = B_mining_table[branch_addr].n_taken_rwcec; // Point to Branch instruction's address, i.e., certain index of mining table
		rem_wcec = rwcec;
#ifdef DEBUG
		cout << endl << "not taken" << endl;
#endif
	}
	else {
		rwcec = B_mining_table[branch_addr].taken_rwcec;
		rem_wcec = rwcec;
#ifdef DEBUG
		cout << endl << "taken" << endl;
#endif
	}
#ifdef DEBUG
	printf("rwcec = %d, ", rwcec); 
#endif
//===============================================================================================================//
	elapsed_time = time_management -> sys_clk -> cur_time - release_time; 
	if(elapsed_time > jitter_config.fin_time_target) {
#ifdef DEBUG
		cout << endl << "Updating Jitter Constraint from " << rep_time_target << "us to " << rep_time_expect << "us" << endl;
#endif
		rep_time_target = rep_time_expect;
	}
        else rep_time_target = jitter_config.fin_time_target;	
	rem_time = rep_time_target - elapsed_time;
	executed_time = time_management -> sys_clk -> cur_time - start_time;
	rep_time_expect = 
				(wcrt - wcet) + // Preempted Duration 
				executed_time + // The total time task has spent so far
				(rwcec / time_management -> sys_clk -> cur_freq); // A future expectation
	target_comparison = rep_time_target - rep_time_expect;
//===============================================================================================================//
	if( (float) target_comparison != 0.0 ) {
#ifdef DEBUG
		printf("cur_block: %d, succ_block: %d\r\n", cur_block_index, succ_block_index);
		printf("Current time: %f us, ", sys_clk -> cur_time);
		printf("Elapsed time = %f us, Target Response Time = %f us, Expected Response Time = %f us, ", 
					elapsed_time, 
					rep_time_target, 
					rep_time_expect
		);
		printf("Difference: %.05f us\r\n", target_comparison);
#endif
		new_freq = (rem_time <= wcrt - elapsed_time) ?  rwcec / rem_time : // Remaining time until WCRT won't lead to deadline miss
								rwcec / (wcrt - elapsed_time); // Deadline miss might occur
		new_freq = (new_freq > max_freq_t) ? max_freq_t : 
		           (new_freq < min_freq_t) ? min_freq_t : new_freq;
	}
	else 
		new_freq = sys_clk -> cur_freq;	
#ifdef DISCRETE_DVFS
	if(new_freq != min_freq_t && new_freq != max_freq_t) new_freq = discrete_handle(new_freq, rwcec);
#endif
	exe_speed_scaling(new_freq);
}

void Src_CFG::L_Intra_task_checkpoint(int cur_block_index, int succ_block_index)
{
	float new_freq;
	float rep_time_target;
	float rep_time_expect; // A speculative execution time if keeping current execution speed withou any change
	float elapsed_time;
	float target_comparison, rem_time, executed_time;
	int rwcec; // Remaining worst-case execution cycles from current basic block
	int loop_index = CFG_path[cur_block_index - 1].L_checkpoint_en[0];
	int loop_addr  = CFG_path[cur_block_index - 1].L_checkpoint_en[1];
//===============================================================================================================//
// Look up the remaining worst-case exeuction cycles (RWCEC) from L-type mining table
	// Accumulate L-type iteration counter by -1 if task reached loop's exit currently
	if(cur_block_index == L_mining_table[loop_index].loop_entry) 
		L_loop_iteration[loop_index].at(ExePathSet_caseID) = 
			((L_loop_iteration[loop_index][ExePathSet_caseID] - 1) == -1) ? 0 : L_loop_iteration[loop_index][ExePathSet_caseID] - 1;
	// Identify the actual branch
	cout << endl << "taken_succ[" << loop_addr << "]: " << L_mining_table[loop_index].taken_succ[loop_addr] << endl;
	cout << "not-taken_succ[" << loop_addr << "]: " << L_mining_table[loop_index].n_taken_succ[loop_addr] << endl;
	if(L_loop_iteration[loop_index][ExePathSet_caseID] == 0) 
		rwcec = L_mining_table[loop_index].succ_rwcec;
	else if( succ_block_index == L_mining_table[loop_index].taken_succ[loop_addr]) {
		rwcec = L_mining_table[loop_index].taken_rwcec[loop_addr] + 
			(L_loop_iteration[loop_index][ExePathSet_caseID] - 1) * P_mining_table[loop_index].iteration_wcec +  
			L_mining_table[loop_index].succ_rwcec; 
#ifdef DEBUG
		cout << endl << "taken\t"; 
		cout << "remaining iteration is (" << loop_index << ")(" << ExePathSet_caseID << "): " << L_loop_iteration[loop_index][ExePathSet_caseID] << endl;
#endif
	}
	else {
		rwcec = L_mining_table[loop_index].n_taken_rwcec[loop_addr] + 
			(L_loop_iteration[loop_index][ExePathSet_caseID] - 1) * P_mining_table[loop_index].iteration_wcec +  
			L_mining_table[loop_index].succ_rwcec; 
#ifdef DEBUG
		cout << endl << "not taken\t"; 
		cout << "remaining iteration is (" << loop_index << ")(" << ExePathSet_caseID << "): " << L_loop_iteration[loop_index][ExePathSet_caseID] << endl;
#endif
	}
	rem_wcec = rwcec;
#ifdef DEBUG
	printf("rwcec = %d, ", rwcec);
#endif
//===============================================================================================================//
	elapsed_time = time_management -> sys_clk -> cur_time - release_time; 
	if(elapsed_time > jitter_config.fin_time_target) {
#ifdef DEBUG
		cout << endl << "Updating Jitter Constraint from " << rep_time_target << "us to " << rep_time_expect << "us" << endl;
#endif
		rep_time_target = rep_time_expect;
	}//
        else rep_time_target = jitter_config.fin_time_target;	
	rem_time = rep_time_target - elapsed_time;
	executed_time = time_management -> sys_clk -> cur_time - start_time;
	rep_time_expect = 
				(wcrt - wcet) + // Preempted Duration 
				executed_time + // The total time task has spent so far
				(rwcec / time_management -> sys_clk -> cur_freq); // A future expectation
	target_comparison = rep_time_target - rep_time_expect;
//===============================================================================================================//
	if( (float) target_comparison != 0.0 ) {
#ifdef DEBUG
		printf("cur_block: %d, succ_block: %d\r\n", cur_block_index, succ_block_index);
		printf("Current time: %.05f us, ", sys_clk -> cur_time);
		printf("Elapsed time = %.05f us, Target Response Time = %.05f us, Expected Response Time = %.05f us, ", 
					elapsed_time, 
					rep_time_target, 
					rep_time_expect
		);
		printf("Difference: %.05f us\r\n", target_comparison);
#endif
		new_freq = (rem_time <= wcrt - elapsed_time) ?  rwcec / rem_time : // Remaining time until WCRT won't lead to deadline miss
								rwcec / (wcrt - elapsed_time); // Deadline miss might occur
		new_freq = (new_freq > max_freq_t) ? max_freq_t : 
		           (new_freq < min_freq_t) ? min_freq_t : new_freq;
		new_freq = (new_freq > max_freq_t) ? max_freq_t : 
		           (new_freq < min_freq_t) ? min_freq_t : new_freq;
	}
	else 
		new_freq = sys_clk -> cur_freq;
#ifdef DISCRETE_DVFS
	if(new_freq != min_freq_t && new_freq != max_freq_t) new_freq = discrete_handle(new_freq, rwcec);
#endif
	exe_speed_scaling(new_freq);

}

void Src_CFG::P_Intra_task_checkpoint(int cur_block_index, int succ_block_index)
{
	float new_freq;
	float rep_time_target;
	float rep_time_expect; // A speculative response time if keeping current execution speed withou any change
	float elapsed_time; 
	float target_comparison, rem_time, executed_time;
	int rwcec; // Remaining worst-case execution cycles from current basic block
	int loop_addr  = CFG_path[cur_block_index - 1].P_checkpoint_en;
//===============================================================================================================//
// Look up the remaining worst-case exeuction cycles (RWCEC) from P-type mining table
	// Since actual loop iteration(s) have been known ahead of task really reach that loop in the near future,
	// the calculation of the remaining worst-case execution cycles is: RWCEC = iteration x Iteration_WCEC + WCEC_after_Loop
	rwcec = P_loop_LaIteration[loop_addr][ExePathSet_caseID] * P_mining_table[loop_addr].iteration_wcec + P_mining_table[loop_addr].succ_rwcec; 
	rem_wcec = rwcec;
#ifdef DEBUG
	cout << endl << endl << "============================================" << endl;
	printf("P-type checkpoint: \r\nblock_%d -> block_%d\r\nrwcec = %d(actual loop iteration: %d)\r\n\r\n, ", cur_block_index, succ_block_index, rwcec, P_loop_LaIteration[loop_addr][ExePathSet_caseID]); 
	cout << endl << endl << "============================================" << endl;
#endif
//===============================================================================================================//
	elapsed_time = time_management -> sys_clk -> cur_time - release_time; 
	if(elapsed_time > jitter_config.fin_time_target) {
#ifdef DEBUG
		cout << endl << "Updating Jitter Constraint from " << rep_time_target << "us to " << rep_time_expect << "us" << endl;
#endif
		rep_time_target = rep_time_expect;
	}
        else rep_time_target = jitter_config.fin_time_target;	
	rem_time = rep_time_target - elapsed_time;
	executed_time = time_management -> sys_clk -> cur_time - start_time;
	rep_time_expect = 
				(wcrt - wcet) + // Preempted Duration 
				executed_time + // The total time task has spent so far
				(rwcec / time_management -> sys_clk -> cur_freq); // A future expectation
	target_comparison = rep_time_target - rep_time_expect;
//===============================================================================================================//
	if( (float) target_comparison != 0.0 ) {
#ifdef DEBUG
		printf("cur_block: %d, succ_block: %d\r\n", cur_block_index, succ_block_index);
		printf("Current time: %f us, ", sys_clk -> cur_time);
		printf("Elapsed time = %f us, Target Response Time = %f us, Expected Response Time = %f us, ", 
					elapsed_time, 
					rep_time_target, 
					rep_time_expect
		);
		printf("Difference: %.05f us\r\n", target_comparison);
#endif
		new_freq = (rem_time <= wcrt - elapsed_time) ?  rwcec / rem_time : // Remaining time until WCRT won't lead to deadline miss
								rwcec / (wcrt - elapsed_time); // Deadline miss might occur
		new_freq = (new_freq > max_freq_t) ? max_freq_t : 
		           (new_freq < min_freq_t) ? min_freq_t : new_freq;
	}
	else 
		new_freq = sys_clk -> cur_freq;	
#ifdef DISCRETE_DVFS
	if(new_freq != min_freq_t && new_freq != max_freq_t) new_freq = discrete_handle(new_freq, rwcec);
#endif
	exe_speed_scaling(new_freq);
}
