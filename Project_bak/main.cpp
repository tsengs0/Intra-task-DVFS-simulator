#include <iostream>
#include "inc/sched.h"
using namespace std;

int main(int argc, char **argv)
{
	float cur_time = 0;

	vector<task_info_t> tasks;
	sys_clk_t sys_clk;
	Ready_Queue que;
	Time_Management time_management(sys_clk);
	tasks.push_back({0.0, /*start_time*/0.0, 1, 3000.0, 605.0, 3000.0, (char) ZOMBIE});
	tasks.push_back({0.0, /*start_time*/0.0, 0, 1000.0, 425.0, 1000.0, (char) ZOMBIE});
	//tasks.push_back({0.0, 0.0, 0, 10.0, 4.0, 10.0, (char) ZOMBIE});
	//tasks.push_back({0.0, 0.0, 1, 10.0, 4.0, 10.0, (char) ZOMBIE});
	//tasks.push_back({0.0, 0.0, 2, 30.0, 6.0, 30.0, (char) ZOMBIE});
	Task_Scheduler task_sched(&time_management, tasks, que, (char) RM);
	
	cout << "task_lise size: " << task_sched.task_list.size() << " " << tasks.size() << endl;

	cout << "==================================================" << endl;
	cout << "\t\t";
	for(int i = 0; i < tasks.size(); i++) cout << "task_" << i << "\t";
	cout << endl << "--------------------------------------------------" << endl;
	time_management.update_cur_time(0.0);
	task_sched.sched_arbitration();
	cout << "0 ns" << endl;
	for(cur_time = 1.0; cur_time <= 3000; cur_time++) {
		for(int i = 0; i < tasks.size(); i++) { 
			if(task_sched.task_list[i].state == (char) RUN) {
				for(int j = 0; j < 15; j++) cout << "-"; 
				for(int j = 0; j < 8*i; j++) cout << "-"; 
				cout << "|" << i << "|";
			}
		} 
		time_management.update_cur_time(cur_time);
		task_sched.sched_arbitration();
		cout << endl << (int) cur_time << " ns\t\t";
		//task_sched.list_task_state();
		cout << endl; // << "--------------------------------------------------" << endl;
	}
	cout << "==================================================" << endl;
	
	return 0;
}

