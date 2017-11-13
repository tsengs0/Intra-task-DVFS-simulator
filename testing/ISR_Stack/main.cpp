#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>

using std::cout;
using std::endl;

typedef struct Task_Element {
	int task_id;
	struct Task_Element *pre_task;
	struct Task_Element *next_task;
} task_element_t;

typedef struct Context {
	int task_id;
	float rwcet; // unit: us
	bool isr_flag;
	struct Context *next; // For stacking
} context_t;

class Preemption_Stack {
	private:
		context_t *top;
		context_t *bottom;
		context_t *ptr;
		int stack_cnt;

	public:
		Preemption_Stack(void);
		~Preemption_Stack(void);
		void push(context_t in);
		void pop(void *inout);
		void stack_list(void);
};

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

int main(int argc, char **argv)
{
	Preemption_Stack isr_stack;
	context_t in_temp;
	
	cout << "Push:" << endl;
	for(int i = 0; i < 10; i++) {
		in_temp.task_id  = i;
		in_temp.rwcet    = i * 2;
		in_temp.isr_flag = false;
		in_temp.next     = NULL;
		isr_stack.push(in_temp);
	}
	isr_stack.stack_list();
	cout << endl << "Pop:" << endl;
	for(int i = 0; i < 10; i++) {
		isr_stack.pop((context_t*) (&in_temp));
		cout << "in_temp(task_id): " << in_temp.task_id << ", ";
		cout << "in_temp(rwcet): " << in_temp.rwcet << ", ";
		if(in_temp.isr_flag == true) cout <<  "in_temp(flag): TRUE" << endl;
		else cout <<  "in_temp(flag): FALSE" << endl;
		isr_stack.stack_list();
		cout << endl;
	}
	return 0;
}
