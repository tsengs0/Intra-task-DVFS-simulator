#include <iostream>
#include <vector>

using std::cout;
using std::cin;
using std::endl;

#define ceiling(x) ( ((x - (int) x) > 0) ? (int) (x + 1) : (int) x )
class RT_Analyser {
	private:
		vector<task_info_t> task_list;

	public:
		RT_Analyser(vector<task_info_t> *tasks);
		¬RT_Analyser(void);

		float RM_Analysis(float wcrt_t);		

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
	vector<task_list> &tasks = task_list;
	float wcrt_t;
	task_id -= 1; //Array starts from index 0

	wcrt_t = tasks[task_id].wcet;
	for(int i = 0; i < tasks.size(); i++) {
		//Task's priority is greater than analysed task's and current iteration is not equal to indicated task index
		if(tasks[i].prt < tasks[task_id].prt && i != task_id)
			wcrt_t += ceiling(wcrt_pre / tasks[i].period) * tasks[i].wcet; 
	}
	
	if(wcrt_t == wcrt_pre) return wcrt_t;
	return RM_Analysis(task_id + 1, wcrt_t);
}


int main(int argc, char **argv)
{



	return 0;
}
