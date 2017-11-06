#include <iostream>
#include <vector>

using std::cout;
using std::cin;
using std::endl;
using std::vector;

#define ZOMBIE 0x01

float ceiling(float x)
{
	return   ((x - (int) x) > 0) ? (int) (x + 1) : (int) x;
}

typedef struct Task_Info {
	float release_time;
	float start_time;
	int prt;
	float rel_dline;
	float wcet; // unit: us
	float period;
	bool NRT_USED; // A flag used to label whether the polling of release_time have been detected/used or not
	char state;
	float  wcrt;
//	Task_Info(int r, int pr, int rd, int wc, int p, char st)
//	: release_time(r), prt(pr), rel_dline(rd), wcet(wc), period(p), state(st) {}
} task_info_t;

class RT_Analyser {
	private:

	public:
		vector<task_info_t> task_list;
		
		RT_Analyser(vector<task_info_t> *tasks);
		~RT_Analyser(void);

		float RM_Analysis(int task_id, float wcrt_pre);		
};

RT_Analyser::RT_Analyser(vector<task_info_t> *tasks) 
{
	task_list = *tasks;	
}

/**
 *  @brief Calculating the worst case response time of certain task by recursive function
 *  @param task_id: the task which is demanded to be analysed
 *  @param wcrt_cur: passing the WCRT of previous iteration
**/
float RT_Analyser::RM_Analysis(int task_id, float wcrt_pre)		
{
	float wcrt_t;
	task_id -= 1; //Array starts from index 0

	wcrt_t = task_list[task_id].wcet;
	for(int i = 0; i < task_list.size(); i++) {
		//Task's priority is greater than analysed task's and current iteration is not equal to indicated task index
		if(task_list[i].prt < task_list[task_id].prt && i != task_id + 1)
			wcrt_t += ceiling(wcrt_pre / task_list[i].period) * task_list[i].wcet; 
	}
	if(wcrt_t == wcrt_pre) return wcrt_t;
	return RM_Analysis(task_id + 1, wcrt_t);
}


int main(int argc, char **argv)
{
	vector<task_info_t> tasks;
	RT_Analyser *rta;

	// Task 1
	tasks.push_back({
			0.0,  // Release Time
			0.0, // Start Time
			0,   // Priority
			7.0, // Relative Deadline 
			3.0, // WCET
			7.0, // Perio
			false,
			ZOMBIE,
			3.0  // Default WCRT
			}
	);

	// Task 2
	tasks.push_back({
			0.0,  // Release Time
			0.0, // Start Time
			1,   // Priority
			12.0, // Relative Deadline 
			3.0, // WCET
			12.0, // Perio
			false,
			ZOMBIE,
			3.0  // Default WCRT
			}
	);
	
	// Task 3
	tasks.push_back({
			0.0,  // Release Time
			0.0, // Start Time
			2,   // Priority
			20.0, // Relative Deadline 
			5.0, // WCET
			20.0, // Perio
			false,
			ZOMBIE,
			5.0  // Default WCRT
			}
	);

	rta = new RT_Analyser(&tasks);

	for(int i = 1; i <= rta -> task_list.size(); i++) {
		cout << "=====================================================" << endl;
		cout << "Releast: " << rta -> task_list[i].release_time << endl;
		cout << "Start: " << rta -> task_list[i].start_time << endl;
		cout << "prt: " << rta -> task_list[i].prt << endl;
		cout << "Relative Deadline: " << rta -> task_list[i].rel_dline << endl;
		cout << "WCET: " << rta -> task_list[i].wcet << endl;
		cout << "Period: " << rta -> task_list[i].period << endl;
		cout << "Default WCRT: " << rta -> task_list[i].wcrt << endl;
		cout << "Analysed WCRT: " << rta -> RM_Analysis((int) i, (float) rta -> task_list[i].wcrt) << endl;	
		cout << "=====================================================" << endl;
	}


	return 0;
}
