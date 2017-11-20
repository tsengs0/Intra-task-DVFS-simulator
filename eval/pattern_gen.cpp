#include "pattern_gen.h"

using std::cin;
using std::cout;
using std::endl;

testbench::testbench(int TskNum, unsigned char branch_level, unsigned char loop_level)
{
	srand(time(NULL));

	tasks_num = TskNum;
	TaskCfg_param = new testbench_param[TskNum];
	for(int i = 0; i < tasks_num; i++) {
		switch(branch_level)
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
				TaskCfg_param[i].N_branch = rand() % ((int) UB_1 + 1 - (int) LB_1) + (int) LB_1;
				break;
		}
		
		switch(loop_level)
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
				TaskCfg_param[i].N_loop = rand() % ((int) UB_1 + 1 - (int) LB_1) + (int) LB_1;
				break;
		}
		
		TaskCfg_param[i].N_BB_loop     = rand() % ((int) UB_BB_loop + 1 - (int) LB_BB_loop) + (int) LB_BB_loop;
		TaskCfg_param[i].N_Branch_loop = rand() % ((int) UB_Branch_loop + 1 - (int) LB_Branch_loop) + (int) LB_Branch_loop;
	}
}

testbench::~testbench()
{

}

void testbench::param_show()
{
	for(int i = 0; i < tasks_num; i++) 
	  cout << "Task " << i << "'s CFG: " << endl
	       << "The number of branches: " << TaskCfg_param[i].N_branch << endl
	       << "The number of loops: " << TaskCfg_param[i].N_loop << endl
	       << "The number of Basic Block inside each loop: " << TaskCfg_param[i].N_BB_loop << endl
	       << "The number of brnaches inside each loop: " << TaskCfg_param[i].N_Branch_loop << endl << endl; 
}
