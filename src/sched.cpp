#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "../inc/sched.h"
#include "../inc/dvfs_info.h"
#include "../inc/main.h"

#ifdef INTRA_SCHED
	extern Task_State_Bus *inter_intra_bus;
#endif

//extern int instance_case[2][3];
//extern int instance_index[2];
//extern int tasks_num;

using std::cout;
using std::cin;
using std::endl;
using std::vector;

Ready_Queue::Ready_Queue(void)
{
	front = NULL; //front -> pre_task = NULL; front -> next_task = NULL;
	rear = NULL; //rear -> pre_task = NULL; rear -> next_task = NULL;
	ptr = NULL; //ptr -> pre_task = NULL; ptr -> next_task = NULL;
}

Ready_Queue::~Ready_Queue(void)
{

}

bool Ready_Queue::IsEmpty(void)
{
	return (front == NULL) ? true : false;
}

int Ready_Queue::get_front(void)
{
	return front -> task_id;
}

int Ready_Queue::get_rear(void)
{
	return rear -> task_id;
}

void Ready_Queue::push(int &new_val)
{
	if(IsEmpty()) {
		front = new task_element_t();
		front -> task_id = new_val;
		front -> pre_task = NULL;
		front -> next_task = NULL;
		rear = front;
		cnt += 1;
#ifdef DEBUG
		cout << "Add first job: Task_" << front -> task_id << " to Read Queue" << endl;
#endif
	}
	else {
		ptr = new task_element_t();
		ptr -> task_id = new_val;
		ptr -> pre_task = NULL;
		ptr -> next_task = front;
		front -> pre_task = ptr;
		front = ptr;
		ptr = NULL;
		cnt += 1;
#ifdef DEBUG
		cout << "Add new job: Task_" << front -> task_id << " to Read Queue" << endl;
#endif
	}
}

/*
	param:
		1) new_val: the new item adding into queue
		2) pre_task: new item's neigbouring element
		3) location: put new item after of before pre_task
*/
void Ready_Queue::insert(int new_val, task_element_t *&pre_task, char location)
{
if(pre_task -> pre_task == NULL) cout << "NULL -> " << pre_task -> task_id << " -> ";
else cout << pre_task -> pre_task -> task_id << " -> " << pre_task -> task_id << " -> ";
if(pre_task -> next_task == NULL) cout << "NULL" << endl;
else cout << pre_task -> next_task -> task_id << endl;
	ptr = new task_element_t();
	ptr -> task_id = new_val;
	if(pre_task == front && location == (char) BEFORE) {
		ptr -> next_task = front;
		ptr -> pre_task = NULL;
		front -> pre_task = ptr;
		cout << "BEFORE(1), " << endl;//front -> task_id << " -> " << front -> next_task -> task_id << endl;
	}
	else if(pre_task == front && location == (char) AFTER) {
		if(pre_task -> pre_task == NULL && pre_task -> next_task == NULL) cout << "Only one, " << pre_task -> task_id << endl;
		ptr -> next_task = front -> next_task;
		ptr -> pre_task =  front;
		front -> next_task = ptr;
		if(ptr -> next_task == NULL) {rear = ptr; cout << "rear -> task_id: " << rear -> task_id << endl;}
		else  {ptr -> next_task -> pre_task = ptr; cout << "111" << endl;}
		cout << "AFTER(1), " << ptr -> task_id << endl;//front -> task_id << " -> " << front -> next_task -> task_id << endl;
	}
	else if(pre_task != front && location == (char) BEFORE){
		ptr -> next_task = pre_task;
		ptr -> pre_task = pre_task -> pre_task;
		pre_task -> pre_task = ptr;
		ptr -> pre_task -> next_task = ptr; 
		cout << "BEFORE(2), " << endl;//ptr ->  task_id << " -> " << ptr -> next_task -> task_id << endl;
	}
	else if(pre_task != front && location == (char) AFTER){
		ptr -> next_task = pre_task -> next_task;
		ptr -> pre_task = pre_task;
		pre_task -> next_task = ptr;
		ptr -> next_task -> pre_task = ptr; 
		cout << "AFTER(2), " << endl;//ptr -> pre_task -> task_id << " -> " << ptr -> task_id << endl;
	}
	
	// Re-locating the Front-pointer
	ptr = front;
	while(ptr -> pre_task != NULL) ptr = ptr -> pre_task;
	front = ptr;
	
	// Re-locating the Rear-pointer
	ptr = rear;
	while(ptr -> next_task != NULL){ ptr = ptr -> next_task; cout << "ptr -> task_id: "<< ptr -> task_id << endl;}
	rear = ptr;
	ptr = NULL;

cout << endl << endl; list_sched_point();
#ifdef DEBUG
	cout << "Insert job: Task_" << new_val << endl;
#endif
}

int Ready_Queue::pop(void)
{
	int temp;

	if(rear == front) {
		temp = rear -> task_id;
		delete front;
		front = NULL;
		rear = NULL;
	}
	else {
		ptr = rear -> pre_task;
		temp = ptr -> next_task -> task_id;
		delete rear;
		rear = ptr;
		rear -> next_task = NULL;
		//ptr = NULL;
	}
#ifdef DEBUG
	cout << "pop task_" << temp << endl;
#endif
	return temp;
}

void Ready_Queue::list_sched_point(void)
{
	ptr = rear;

	if(ptr == NULL) cout << "Empty in the Ready Queue" << endl;
	else if(front == rear) cout << "Rear and Front: Task_" << ptr -> task_id << endl;
	else {
		cout << "Rear:  Task_" << ptr -> task_id << endl; ptr = ptr -> pre_task;
		while(ptr != front) {
			cout << "Task_" << ptr -> task_id << endl;
			ptr = ptr -> pre_task;
		}
		cout << "Front: Task_" << ptr -> task_id << endl;
	}

}
////////////////////////////////////////////////////////////////////////////////////////////////////
float ceiling(float x)
{
	return   ((x - (int) x) > 0) ? (int) (x + 1) : (int) x;
}

RT_Analyser::RT_Analyser(task_info_t *tasks_list) 
{
	tasks = tasks_list;	
}

/**
 *  @brief Calculating the worst case response time of certain task by recursive function
 *  @param task_id: the task which is demanded to be analysed
 *  @param wcrt_cur: passing the WCRT of previous iteration
**/
float RT_Analyser::RM_Analysis(int task_id, float wcrt_pre)		
{
	float wcrt_t;
	wcrt_t = tasks[task_id].wcet;
	for(int i = 0; i < tasks_num; i++) {
		//Task's priority is greater than analysed task's and current iteration is not equal to indicated task index
		if(tasks[i].prt < tasks[task_id].prt && i != task_id)
			wcrt_t += ceiling(wcrt_pre / tasks[i].period) * tasks[i].wcet; 
	}
	if(wcrt_t == wcrt_pre) return wcrt_t;
	return RM_Analysis(task_id, wcrt_t);
}

Task_Scheduler::Task_Scheduler(Time_Management *timer, task_info_t *tasks, Ready_Queue &queue, char policy, Task_State_Bus *&msg_bus)
{
	time_management = timer;
	task_list = tasks;
	ready_queue = &queue;
	sched_policy = policy;
	running_task_id = (int) CPU_IDLE;
	pre_task = running_task_id;
	new_task_start_flag = false;
	inter_intra_bus = msg_bus;
	rwcet = (float) 0.0;
	cur_context.task_id = (int) NO_PREEMPTION;
	cur_context.rwcet = (float) 0.0;
	cur_context.isr_flag = false;	
	
	// Creating Response-Time-Analyser module
	rta = new RT_Analyser(task_list);
	for(int i = 0; i < tasks_num; i++) {
		float wcrt_t = rta -> RM_Analysis(i, task_list[i].wcrt);
		task_list[i].wcrt = wcrt_t;
		inter_intra_bus -> intra_tasks[i].wcrt = wcrt_t;
		cout << "Worst-Case Response Time (WCRT) of Task_" 
		     << i << ": " 
		     << inter_intra_bus -> intra_tasks[i].wcrt << " us" << endl;
	}
	// Initially, all tasks' state are set to 'IDLE'
	for(int i = 0; i < tasks_num; i++) task_list[i].state = (char) IDLE;
}

Task_Scheduler::~Task_Scheduler(void)
{

}

void Task_Scheduler::resume(void)
{
	isr_stack.pop((Preemption_Stack*) (&cur_context));
	rwcet = cur_context.rwcet;
#ifdef DEBUG
	cout << "Resume Task_" << cur_context.task_id << "'s execution(RWCET:" << rwcet << " us) " << "from Task_" << running_task_id << endl;
	char a; cout << "Press any key:"; cin >> a; 
#endif
}

/*
 Since it is just a simulation, so the preemption will be behaved as a recursion,
 and the behaviour of contex switch in terms of simulation will be:
 1) Intentially put some delay
 2) Starting running another/new task with the highest priority
*/
void Task_Scheduler::context_switch(int &cur_task, int &new_task)
{
	bool sched_verify = true;
	// (1) delay()
	// (2) Src_CFG[new_task].traverse_spec_path()
	
	// Fill current context into Preemption Stack
	cur_context.task_id = cur_task;
	cur_context.rwcet = rwcet;
	cur_context.isr_flag = true;	
	isr_stack.push(cur_context);
	
	rwcet = task_list[new_task].wcet;
	pre_task = (int) CPU_IDLE;
/*
	// Suspending the running task temporarily, and putting it into the rear of Ready Queue
	task_list[cur_task].state = (char) READY;
	ready_queue -> insert(cur_task, ready_queue -> rear, (char) AFTER);

	if(sched_policy == (char) RM) sched_verify = RM_sched(cur_task);
	else if(sched_policy == (char) EDF) sched_verify = EDF_sched(cur_task);
*/
	list_task_state();
#ifdef DEBUG
	cout << "Preemption!" << endl;
#endif

}

Preemption_Stack::Preemption_Stack(void)
{
	top = NULL;
	bottom = NULL;
	ptr = NULL;
	stack_cnt = 0;
}

Preemption_Stack::~Preemption_Stack(void)
{
	stack_cnt = 0;
/*	if(top == NULL) delete top;
	if(bottom == NULL) delete bottom;
	if(ptr == NULL) delete ptr;
*/
}

void Preemption_Stack::push(context_t in)
{
	ptr = new context_t;
	ptr -> task_id = in.task_id;
	ptr -> rwcet = in.rwcet;
	ptr -> isr_flag = in.isr_flag;

	if(bottom == NULL) {
		if(top != NULL) {
			cout << "Wrong Stack Manipulation" << endl;
			cout << "Since Bottom is pointing to the NULL, so Top also should be located in NULL" << endl;
			exit(1);
		}
		else {
			top = ptr; bottom = ptr;
			bottom -> next = NULL;
		}
	}
	else {
		ptr -> next = top;
		top = ptr;
	}
	stack_cnt += 1;
}

void Preemption_Stack::pop(void *inout)
{
	context_t *out = (context_t*) inout;

	if(bottom == NULL) {
		if(top != NULL) {
			cout << "Wrong Stack Manipulation" << endl;
			cout << "Since Bottom is pointing to the NULL, so Top also should be located in NULL" << endl;
			exit(1);
		}
		else {
			cout << "You use Preemption Stack at improper time" << endl;
			cout << "Because there is no context inside Preemption Stack" << endl;
			exit(1);
		}
	}
	else {
		out -> task_id  = top -> task_id;
		out -> rwcet    = top -> rwcet; // unit: us
		out -> isr_flag = top -> isr_flag;
		out -> next     = NULL;
	}
	stack_cnt -= 1;
	ptr = top;
	top = top -> next;
	delete ptr;
}

void Preemption_Stack::stack_list(void)
{
	ptr = top;
	cout << "Top(Input/Output) -> ...... -> Bottom" << endl;
	for(int i = 0; i < stack_cnt; i++) {
		cout << "Task_" << ptr -> task_id << " -> ";
		if(i != stack_cnt - 1) ptr = ptr -> next;
		else cout << "NULL" << endl;
	}
}

bool Preemption_Stack::IsEmpty(void)
{
	return (top == NULL) ? true : false;
}

void Task_Scheduler::sched_arbitration(float sched_tick)
{
	bool sched_verify = true;

	// The completion of currently running task
#ifdef INTRA_SCHED
	if(IsIdle() != true && inter_intra_bus -> intra_tasks[running_task_id].completion_flag == true) {
		task_list[running_task_id].state = (char) IDLE;
		task_list[running_task_id].release_time += task_list[running_task_id].period;
		inter_intra_bus -> intra_tasks[running_task_id].completion_flag = false;
		task_list[running_task_id].NRT_USED = false;
		if(isr_stack.IsEmpty() == false) resume();
		else {
			cur_context.task_id = (int) NO_PREEMPTION;
			cur_context.rwcet = (float) 0.0;
			cur_context.isr_flag = false;
		}
	#ifdef DEBUG
		cout << endl << "Task_" << running_task_id << " complete at " << time_management -> sys_clk -> cur_time << " us" << endl;
		cout << "Task_" << running_task_id << "'s next release time: " << task_list[running_task_id].release_time << " us" << endl;
	#endif
		running_task_id = (int) CPU_IDLE;
	}
	else if(IsIdle() != true && inter_intra_bus -> intra_tasks[running_task_id].completion_flag == false) {
		rwcet = (inter_intra_bus -> intra_tasks[running_task_id].rem_wcec) / (time_management -> sys_clk -> cur_freq); // Conservative estimation
	}
#else
	if(IsIdle() != true && rwcet == 1) {
		task_list[running_task_id].state = (char) IDLE;
		task_list[running_task_id].release_time += task_list[running_task_id].period;
		running_task_id = (int) CPU_IDLE;
	}
	else if(IsIdle() != true && rwcet != 1) {
		 rwcet -= 1;
	}
#endif
	// Filling the arrival task in Ready Queue
	// In the real processor, we only need to deal with the parameter(index of arrival task, etc.) given from interruption,
	// instead of travesing whole task set to see whether there is a new request for executing a certain task
	for(int i = 0; i < tasks_num; i++) {
		if(
			task_list[i].release_time <= time_management -> sys_clk -> cur_time &&
			task_list[i].NRT_USED == false
		) { 
			task_list[i].NRT_USED = true;
			cout << "Task_" << i << " release" << endl;
			if(IsIdle() == true && ready_queue -> IsEmpty() == true) {
#ifdef DEBUG
				cout << "Idle and Empty-queue: " << i << endl;
#endif
				task_list[i].state = (char) READY;
				ready_queue -> push(i);
				//pre_task = running_task_id;
			}
			else if(IsIdle() == true && ready_queue -> IsEmpty() != true) {
#ifdef DEBUG
				cout << "Idle and non-Empty-queue: " << i << endl;
#endif
				task_list[i].state = (char) READY;
				//pre_task = running_task_id;

				if(sched_policy == (char) RM) sched_verify = RM_sched(i);
				else if(sched_policy == (char) EDF) sched_verify = EDF_sched(i);
			}
			else if(IsIdle() != true && ready_queue -> IsEmpty() == true) {
#ifdef DEBUG
				cout << "non-Idle and Empty-queue" << endl;
#endif
				task_list[i].state = (char) READY;

				// Suspending the running task temporarily, and putting it into the rear of Ready Queue
				task_list[running_task_id].state = (char) READY;
				ready_queue -> push(running_task_id);
				pre_task = running_task_id;
				running_task_id = (int) CPU_IDLE;

				if(sched_policy == (char) RM) sched_verify = RM_sched(i);
				else if(sched_policy == (char) EDF) sched_verify = EDF_sched(i);
			}
			else{ // IsIdle() != true && ready_queue -> IsEmpty() == false
#ifdef DEBUG
				cout << "non-Idle: " << i << endl;
#endif
				task_list[i].state = (char) READY;

				// Suspending the running task temporarily, and putting it into the rear of Ready Queue
				task_list[running_task_id].state = (char) READY;
				
				cout << "1" << endl;
				ready_queue -> insert(running_task_id, ready_queue -> rear, (char) AFTER);
				cout << "2" << endl;
				pre_task = running_task_id;
				running_task_id = (int) CPU_IDLE;
				
				cout << "3" << endl;
				if(sched_policy == (char) RM) sched_verify = RM_sched(i);
				else if(sched_policy == (char) EDF) sched_verify = EDF_sched(i);
				cout << "4" << endl;
			}
		}
	}
	if(IsIdle() == true && ready_queue -> IsEmpty() != true) dispatcher();
}

bool Task_Scheduler::RM_sched(int &task_new)
{
	task_element_t *ptr = ready_queue -> rear;
	// Rearranging the order of tasks basing on their priorities
	while(1) {
		if(ptr == ready_queue -> front) break;
		else if(task_list[task_new].prt < task_list[ptr -> task_id].prt) break;
		else ptr = ptr -> pre_task;
	}
	ready_queue -> insert(
				task_new,
				ptr,
				(task_list[task_new].prt > task_list[ptr -> task_id].prt) ?
				(char) BEFORE : (char) AFTER
			);

	return SCHEDULABLE;
}

bool Task_Scheduler::EDF_sched(int &task_new)
{

	// Rearranging every task's priority
	return SCHEDULABLE;
}

void Task_Scheduler::dispatcher(void)
{
	//int pre_task;

	//pre_task = running_task_id;
	running_task_id = ready_queue -> pop();
	task_list[running_task_id].state = (char) RUN;
#ifdef DEBUG
	cout << "running_task: Task_" << running_task_id << endl;
#endif
	// Checking whether context switch is needed or not, e.g., preemption or comletion/start is happened
	if(pre_task != running_task_id && pre_task != (int) CPU_IDLE) { // Preemption
		cout << "preemption(pre:" << pre_task << ", new:" << running_task_id << ")" << endl;
		context_switch(pre_task, running_task_id);
		char a; cout << "Press any key:"; cin >> a; 
		
		cout << "start new task" << endl; 
		rwcet = task_list[running_task_id].wcet;
		new_task_start_flag = true;
		inter_intra_bus -> start_new_task_config(
					running_task_id, 
					rand() % ((int) patterns_num),
					(int) WORST, 
					task_list[running_task_id].release_time, 
					task_list[running_task_id].start_time, 
					task_list[running_task_id].period, 
					(char) DVFS_ENABLE
				);
		new_task_start_flag = false;
	} 
	else if(cur_context.isr_flag == true && cur_context.task_id == running_task_id) { // Resuming from previous preemption
		cout << "resume" << endl; 
		//resume();
	} 
	else { // Starting new arrival task
		cout << "start new task" << endl; 
		rwcet = task_list[running_task_id].wcet;
		new_task_start_flag = true;
		inter_intra_bus -> start_new_task_config(
					running_task_id, 
					rand() % ((int) patterns_num), 
					(int) WORST, 
					task_list[running_task_id].release_time, 
					task_list[running_task_id].start_time, 
					task_list[running_task_id].period, 
					(char) DVFS_ENABLE
				);
		new_task_start_flag = false;
	} 
#ifdef DEBUG
//	list_task_state();
#endif
}

bool Task_Scheduler::IsIdle(void)
{
	return (running_task_id == (int) CPU_IDLE) ? true : false;
}

void Task_Scheduler::list_task_state(void)
{
        cout << "===================================" << endl;
        cout << "Current Time: " << (int) (time_management -> sys_clk -> cur_time) << " s" << endl;
        for(int i = 0; i < tasks_num; i++) {
                switch(task_list[i].state) {
                        case (char) READY:
                                cout << "Task_" << i << ": " << "READY";
                                break;
                        case (char) WAIT:
                                cout << "Task_" << i << ": " << "WAIT";
                                break;
                        case (char) IDLE:
                                cout << "Task_" << i << ": " << "IDLE";
                                break;
                        case (char) RUN:
                                cout << "Task_" << i << ": " << "RUN";
                                break;
                        case (char) ZOMBIE:
                                cout << "Task_" << i << ": " << "ZOMBIE";
                                break;
                }
		printf("  (Absolute Deadline: %.02f, Release time: %.02f)\r\n",
				task_list[i].rel_dline + task_list[i].release_time,
				task_list[i].release_time
		);
        }

        cout << endl << "#Ready Queue:" << endl;
        ready_queue -> list_sched_point();
        cout << "===================================" << endl;

}

