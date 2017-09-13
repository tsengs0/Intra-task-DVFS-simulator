#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "../inc/sched.h"
#include "../inc/dvfs_info.h"

#ifdef INTRA_SCHED
	extern Task_State_Bus *inter_intra_bus;
#endif

extern int instance_case[2][3];
extern int instance_index[2];
extern int tasks_num;

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
	ptr = new task_element_t();
	ptr -> task_id = new_val;
	if(pre_task == front && location == (char) BEFORE) {
		ptr -> next_task = front;
		ptr -> pre_task = NULL;
		front -> pre_task = ptr;
		front = ptr;
	}
	else if(pre_task == front && location == (char) AFTER) {
		ptr -> next_task = front -> next_task;
		ptr -> pre_task =  front;
		front -> next_task = ptr;
		(ptr -> next_task == NULL) ? ptr = rear : ptr -> next_task -> pre_task = ptr;
	}
	else if(pre_task != front && location == (char) BEFORE){
		ptr -> next_task = pre_task;
		ptr -> pre_task = pre_task -> pre_task;
		pre_task -> pre_task = ptr;
		ptr -> pre_task -> next_task = ptr;
	}
	else if(pre_task != front && location == (char) AFTER){
		ptr -> next_task = pre_task -> next_task;
		ptr -> pre_task = pre_task;
		pre_task -> next_task = ptr;
		ptr -> next_task -> pre_task = ptr;
	}

	// Re-locating the Front-pointer
	ptr = front;
	while(ptr -> pre_task != NULL) ptr = ptr -> pre_task;
	front = ptr;

	// Re-locating the Rear-pointer
	ptr = rear;
	while(ptr -> next_task != NULL) ptr = ptr -> next_task;
	rear = ptr;
	ptr = NULL;
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
		temp = rear -> task_id;
		ptr = rear -> pre_task;
		delete rear;
		rear = ptr;
		ptr = NULL;
	}
#ifdef DEBUG
	cout << "pop" << endl;
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
	isr_stack.task_id = (int) NO_PREEMPTION;
	isr_stack.rwcet = (float) 0.0;
	isr_stack.isr_flag = false;	
	
	// Initially, all tasks' state are set to 'IDLE'
	for(int i = 0; i < tasks_num; i++) task_list[i].state = (char) IDLE;
}

Task_Scheduler::~Task_Scheduler(void)
{

}

void Task_Scheduler::resume(void)
{
	isr_stack.task_id = (int) NO_PREEMPTION;
	rwcet = isr_stack.rwcet;

	isr_stack.rwcet = (float) 0.0;
	isr_stack.isr_flag = false;

#ifdef DEBUG
	cout << "Resume!" << endl;
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
	isr_stack.task_id = cur_task;
	isr_stack.rwcet = rwcet;
	isr_stack.isr_flag = true;
	rwcet = task_list[new_task].wcet;
	pre_task = (int) CPU_IDLE;
/*
	// Suspending the running task temporarily, and putting it into the rear of Ready Queue
	task_list[cur_task].state = (char) READY;
	ready_queue -> insert(cur_task, ready_queue -> rear, (char) AFTER);

	if(sched_policy == (char) RM) sched_verify = RM_sched(cur_task);
	else if(sched_policy == (char) EDF) sched_verify = EDF_sched(cur_task);
	//list_task_state();
#ifdef DEBUG
	cout << "Preemption!" << endl;
#endif
*/
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
	#ifndef DEBUG
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
				ready_queue -> insert(running_task_id, ready_queue -> rear, (char) AFTER);
				pre_task = running_task_id;
				running_task_id = (int) CPU_IDLE;

				if(sched_policy == (char) RM) sched_verify = RM_sched(i);
				else if(sched_policy == (char) EDF) sched_verify = EDF_sched(i);
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
		
		cout << "start new task" << endl; 
		rwcet = task_list[running_task_id].wcet;
		new_task_start_flag = true;
		inter_intra_bus -> start_new_task_config(
					running_task_id, 
					instance_case[running_task_id][instance_index[running_task_id]++], //rand() % 6, 
					(int) WORST, 
					task_list[running_task_id].release_time, 
					task_list[running_task_id].start_time, 
					task_list[running_task_id].period, 
					(char) DVFS_ENABLE
				);
		new_task_start_flag = false;
	} 
	else if(isr_stack.isr_flag == true && isr_stack.task_id == running_task_id) { // Resuming from previous preemption
		cout << "resume" << endl; 
		resume();
	} 
	else { // Starting new arrival task
		cout << "start new task" << endl; 
		rwcet = task_list[running_task_id].wcet;
		new_task_start_flag = true;
		inter_intra_bus -> start_new_task_config(
					running_task_id, 
					instance_case[running_task_id][instance_index[running_task_id]++], //rand() % 6, 
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

