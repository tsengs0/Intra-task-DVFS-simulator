#include "pattern_gen.h"

using std::cin;
using std::cout;
using std::endl;
using std::vector;

testbench::testbench(int TskNum, task_info_t *task_set)
{
	tasks = task_set;
	N_init = new int[TskNum];
	BB_max = new int[TskNum];
	BB_min = new int[TskNum];
	BB_avg = new int[TskNum];
	wcec   = new int[TskNum]; 
	for(int i = 0; i < TskNum; i++) {
		N_init[i] = rand() % ((int) UB_BB_num + 1 - (int) LB_BB_num) + (int) LB_BB_num;
		wcec[i] = task_set[i].wcet * (int) default_freq;
		BB_max[i] = wcec[i] / N_init[i];
		BB_min[i] = 5;
	}
}

testbench::~testbench()
{

}

void testbench::CfgParam_config(int TskNum, unsigned char *branch_level, unsigned char *loop_level)
{
	srand(time(NULL));
	tasks_num = TskNum;
	TaskCfg_param = new testbench_param[TskNum];
	for(int i = 0; i < tasks_num; i++) {
		switch(branch_level[i])
		{
			case (unsigned char) 1:
				TaskCfg_param[i].N_branch = rand() % ((int) UB_1 + 1 - (int) LB_1) + (int) LB_1;
				break;
			case (unsigned char) 2:
				TaskCfg_param[i].N_branch = rand() % ((int) UB_2 + 1 - (int) LB_2) + (int) LB_2;
				break;
			case (unsigned char) 3:
				TaskCfg_param[i].N_branch = rand() % ((int) UB_3 + 1 - (int) LB_3) + (int) LB_3;
				break;
			case (unsigned char) 4:
				TaskCfg_param[i].N_branch = rand() % ((int) UB_4 + 1 - (int) LB_4) + (int) LB_4;
				break;
			default:
				TaskCfg_param[i].N_branch = 0;
				break;
		}
		
		switch(loop_level[i])
		{
			case (unsigned char) 1:
				TaskCfg_param[i].N_loop = rand() % ((int) UB_1 + 1 - (int) LB_1) + (int) LB_1;
				break;
			case (unsigned char) 2:
				TaskCfg_param[i].N_loop = rand() % ((int) UB_2 + 1 - (int) LB_2) + (int) LB_2;
				break;
			case (unsigned char) 3:
				TaskCfg_param[i].N_loop = rand() % ((int) UB_3 + 1 - (int) LB_3) + (int) LB_3;
				break;
			case (unsigned char) 4:
				TaskCfg_param[i].N_loop = rand() % ((int) UB_4 + 1 - (int) LB_4) + (int) LB_4;
				break;
			default:
				TaskCfg_param[i].N_loop = 0;
				break;
		}
		
		TaskCfg_param[i].N_Branch_loop = rand() % ((int) UB_Branch_loop + 1 - (int) LB_Branch_loop) + (int) LB_Branch_loop;
		TaskCfg_param[i].N_BB_loop     = TaskCfg_param[i].N_Branch_loop * 3;//rand() % ((int) UB_BB_loop + 1 - (int) LB_BB_loop) + (int) LB_BB_loop;
	}
}

void testbench::cfg_gen(int TskID)
{
	int fsm; // 0: normal BB, 1: branch, 2: loop entry, 3: loop exit
	int wcec_sum = wcec[TskID], BB_num = N_init[TskID], wcec_temp;
	int bb_max = BB_max[TskID], bb_min = BB_min[TskID], bb_avg = BB_avg[TskID]; 
	int cur_id = 1;
	vector<int> succ_temp;
	BB *BB_temp;
	
	// Set start node as a normal BB
	BB_temp = new BB(cur_id, (rand() % (bb_max + 1 - bb_min) + bb_min), 0, 0, succ_temp);
	ExePath_case.push_back(*BB_temp); vector<int>().swap(succ_temp); delete BB_temp;

	while(wcec_sum != 0 || BB_num != 0) { // Keep topologising as long as budget(e.g., available WCEC or available number of Basic Blocks) hasn't been run out
		switch(fsm)
		{
			case 0: // Normal BB
				wcec_temp = (bb_max <= wcec_sum) ? (rand() % (bb_max + 1 - bb_min) + bb_min) : wcec_sum; 
				BB_temp = new BB(
						cur_id, 
						wcec_temp,
						0, 0, 
						succ_temp
					);
				ExePath_case.push_back(*BB_temp); vector<int>().swap(succ_temp); delete BB_temp;
				wcec_sum -= wcec_temp;
				cur_id += 1;
				break;
			case 1: // Branch
								
				break;
			case 2: // Loop Entry
			
				break;
			case 3: // Loop Exit
			
				break;
			default:
			
				break;

		}
	}
}

void testbench::param_show()
{
	for(int i = 0; i < tasks_num; i++) 
	  cout << "Task " << i << "'s CFG: " << endl
	       << "The number of branches: " << TaskCfg_param[i].N_branch << endl
	       << "The number of loops: " << TaskCfg_param[i].N_loop << endl
	       << "The maximal number of Basic Blocks inside every loop:" << TaskCfg_param[i].N_BB_loop << endl
	       << "The maximal number of brnaches inside every loop: " << TaskCfg_param[i].N_Branch_loop << endl 
	       << "The worst-case execution cycles (WCECs): " << wcec[i] << endl
	       << "The number of Basic Blocks: " << N_init[i] << endl
	       << "BB_max: " << BB_max[i] << endl
	       << "BB_min: " << BB_min[i] << endl << endl; 
}
