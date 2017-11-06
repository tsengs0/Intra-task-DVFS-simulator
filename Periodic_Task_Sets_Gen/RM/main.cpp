#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iomanip>
#include "pattern_gen.h"

using std::cout;
using std::cin;
using std::endl;
using std::setw;

int tasks_num;
enum {
	READY  = 1,
	WAIT   = 2,
	IDLE   = 3,
	RUN    = 4,
	ZOMBIE = 5
};

void taskSet_gen(task_info_t *tasks, int tskNum, float *wcet_bound, float perct_util, unsigned char sched_id, char *output_FileName);
float RandomFloat(float LowerBound, float UpperBound);

/**
  * @brief Executing/simulating the sub-portion of control flow graph in terms of
  *	   interrupt timer's period(preloaded cycle count). For sake of simulating
  *        preemptive behaiour
  * @param case_t: designating the estimated 1)worst-case, 2)average-case,
  *        3)best-case to entire execution
  * @param DVFS_en: enable or disable DVFS mechanism
**/
void util_calculation(task_info_t *task_set, int tasks_num, float *util)
{
    float util_temp = 0;
    for(int i = 0; i < tasks_num; i++) 
        util_temp += (task_set[i].wcet / task_set[i].period);
    *util = util_temp;
}

/**
  * @brief Executing/simulating the sub-portion of control flow graph in terms of
  *	   interrupt timer's period(preloaded cycle count). For sake of simulating
  *        preemptive behaiour
  * @param case_t: designating the estimated 1)worst-case, 2)average-case,
  *        3)best-case to entire execution
  * @param DVFS_en: enable or disable DVFS mechanism
**/
float util_bound(int num)
{
    return num * (pow(2, (float) 1 / num) - 1);
}

/**
  * @brief Executing/simulating the sub-portion of control flow graph in terms of
  *	   interrupt timer's period(preloaded cycle count). For sake of simulating
  *        preemptive behaiour
  * @param case_t: designating the estimated 1)worst-case, 2)average-case,
  *        3)best-case to entire execution
  * @param DVFS_en: enable or disable DVFS mechanism
**/
bool schedulability(int TskNum, float util)
{
    return (util <= util_bound(TskNum)) ? true : false;
}

int main(int argc, char **argv)
{
	char txt[50];
	float util;
	float wcet_bound[2];
	int tasks_num;
	unsigned char *branch_level, *loop_level;
	task_info_t *src_inter;
	testbench *tb;
	srand(time(NULL));

	// Create the generated Periodic task set's text file
	sprintf(txt, "echo \"Scheduler: RM\" > %s", argv[1]);
	system(txt); 

//==================================================================================================//
	cout << "Designate the number of Tasks" << endl;
	cin >> tasks_num;
	src_inter = new task_info_t[tasks_num];
	wcet_bound[1] = RandomFloat(1000, 10000);
	wcet_bound[0] = wcet_bound[1] / 10;
	taskSet_gen(
		src_inter,
		(int) tasks_num,
		wcet_bound,
		(float) 1.0,
		(unsigned char) 0x01,
		argv[1]
	);

	cout << "Task_ID" << setw(20) <<  "WCET(us)" << setw(20) << "Deadline(us)" << setw(20) << "Period(us)" << endl;
	cout << "------------------------------------------------------------------------------" << endl;
	for(int i = 0; i < tasks_num; i++) {
	  cout << i << setw(20)
               << (unsigned int) src_inter[i].wcet << setw(20)
               << (unsigned int) src_inter[i].rel_dline << setw(20)
               << (unsigned int) src_inter[i].period << endl;
	}
	cout << endl << "Utilisation bound for RM scheduler with periodic task-sets: "
	     << util_bound(tasks_num)
	     << endl;

	util_calculation(src_inter, tasks_num, &util);
	cout << "Utilisation: "
	     << util << endl;

	if(schedulability(tasks_num, util)) cout << "Schedulable!" << endl;
	else { cout << "Not Schedulable" << endl; exit(0); }
	cout << endl;
//==================================================================================================//
	
	branch_level     = new unsigned char[tasks_num]; 
	loop_level       = new unsigned char[tasks_num]; 
	for(int i = 0; i < tasks_num; i++) {
	  branch_level[i] = (unsigned char) (rand() % 4 + 1); 
	  loop_level[i]   = (unsigned char) (rand() % 4 + 1); 
	}
	tb = new testbench(tasks_num, src_inter);
	tb -> CfgParam_config(tasks_num, branch_level, loop_level);
	tb -> param_show();

	return 0;
}

float RandomFloat(float LowerBound, float UpperBound)
{
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = UpperBound - LowerBound;
    float r = random * diff;
    return LowerBound + r;
}

void taskSet_gen(
                 task_info_t *tasks,
                 int tskNum,
                 float *wcet_bound,
                 float perct_util,
                 unsigned char sched_id,
		 char *output_FileName
)
{
    char txt[50];
    float bound = util_bound(tskNum);
    float util = bound * perct_util;
    float util_div = util / tskNum;

    for(int i = 0; i < tskNum; i++) {
            // Randomly assign an utilisation, i.e., WCET / Period
            float util_temp = RandomFloat(0.0, util);
            tasks[i].wcet = (unsigned int) RandomFloat(wcet_bound[0], wcet_bound[1]);
            tasks[i].period = (unsigned int) tasks[i].wcet / util_temp;
            tasks[i].rel_dline = (unsigned int) tasks[i].period;
            util -= util_temp;
    }
    
	for(int i = 0; i < tskNum; i++) {
	  sprintf(txt, "echo \"{%u,%u,%u,%u}\" >> %s", (unsigned int) i, (unsigned int) tasks[i].wcet, (unsigned int) tasks[i].rel_dline, (unsigned int) tasks[i].period, output_FileName); 
	  system(txt);
	}
}
