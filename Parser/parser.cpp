#include "parser.h"

using namespace std;

const char *checkpoint_file = "checkpoint.txt";
const string checkpoint_id_1("Task_Numbers:");
const string checkpoint_id_2("Task_ID:");
const string checkpoint_id_3("Checkpoint_Numbers:");
const string checkpoint_id_4("B");
const string checkpoint_id_5("L");
const string checkpoint_id_6("P");
const string checkpoint_id_end("#");

Parser::Parser(void)
{
}

Parser::~Parser(void)
{
}

void Parser::checkpoint_in(int tsk_num_in, RWCEC_Trace_in *cycle_trace, checkpoint_num *checkpoint_numbers, checkpoints_t *checkpoint_BlockID)
{
	int fsm = (int) TSK_NUM;
	string path(checkpoint_file);
	string read_line;
	string read_id;
	string read_id_1;
	string read_int;
	int tasks_num, TaskNum_check = 0, BNum_check, LNum_check, PNum_check;
	int task_id;
	char *cstr;
	ifstream file_in(path);

	if(!file_in) {
		cout << "There no checkpoint file in current directory, expecting " 
		     << checkpoint_file << endl;
		exit(1);
	}

	//while(!file_in.eof()) {
	while(1) {
		getline(file_in, read_line);
		stringstream read_content(read_line);		
		read_content >> read_id;
		if(!read_id.compare(checkpoint_id_end)) {
			if(TaskNum_check > tasks_num) {
				cout << "The input Task numbers("
				     << TaskNum_check << " tasks)"
				     << " exceed the pre-defined Number of Tasks(" 
				     << tasks_num << " tasks)" << endl;
				exit(1);
			}
			else if(TaskNum_check > tasks_num) {
				cout << "The pre-defined number of Tasks is "
				     << tasks_num << ", but only "
				     <<	TaskNum_check << " tasks have been configured" << endl;
				exit(1);
			}
			else break;
		}

		switch(fsm) {
			case (int) TSK_NUM:
				if(!read_id.compare(checkpoint_id_1)) {
					read_content >> read_int;
					tasks_num = char_int(read_int.c_str());
					if(tasks_num == tsk_num_in) {
						cycle_trace = new RWCEC_Trace_in[tasks_num];
						checkpoint_numbers = new checkpoint_num[tasks_num];
						checkpoint_BlockID = new checkpoints_t[tasks_num];
		
					}
					else {
						cout << "According to the Task-set-defined file, the number of tasks suppose be " 
						     << tsk_num_in << "." << endl 
						     << "However, there are only " 
						     << tasks_num << " number of tasks defined in Checkpoint-defined file." << endl;
						exit(1);
					}
				}
				else {
					cout << "Wrong syntax \"" << read_id << "\"" << endl;
					cout << "The number of Tasks ought to be defined" << endl;
					exit(1);
				}
				cout << checkpoint_id_1 << " " << tasks_num << endl;
				fsm = (int) TSK_ID;
				break;

			case (int) TSK_ID:
				if(!read_id.compare(checkpoint_id_2)) {
					read_content >> read_int;
					task_id = char_int(read_int.c_str());
					BNum_check = 0; LNum_check = 0; PNum_check = 0; 
					TaskNum_check += 1;
				}
				else {
					cout << "Wrong syntax \"" << read_id << "\"" << endl;
					cout << "The Task ID ought to be defined" << endl;
					exit(1);
				}		
				cout << checkpoint_id_2 << " " << task_id << endl;	
				fsm = (int) CH_NUM;
				break;
 
			case (int) CH_NUM:
				if(!read_id.compare(checkpoint_id_3)) {
					read_content >> read_int;
					checkpoint_numbers[task_id - 1].B_ch = char_int(read_int.c_str());
					cycle_trace[task_id - 1].B_RWCEC_t = new B_type[checkpoint_numbers[task_id - 1].B_ch];
					
					read_content >> read_int;
					checkpoint_numbers[task_id - 1].L_ch = char_int(read_int.c_str());
					cycle_trace[task_id - 1].L_RWCEC_t = new L_type[checkpoint_numbers[task_id - 1].L_ch];
					
					read_content >> read_int;
					checkpoint_numbers[task_id - 1].P_ch = char_int(read_int.c_str());
					cycle_trace[task_id - 1].P_RWCEC_t = new P_type[checkpoint_numbers[task_id - 1].P_ch];
				}
				else {
					cout << "Wrong syntax \"" << read_id << "\"" << endl;
					cout << "Wrong format for defining the numbers of each type of Checkpoint" << endl;
					exit(1);
				}		
				cout << checkpoint_id_3 << " B(" << checkpoint_numbers[task_id - 1].B_ch 
				     << ") L(" << checkpoint_numbers[task_id - 1].L_ch
				     << ") P(" << checkpoint_numbers[task_id - 1].P_ch << ")" <<endl;	
				fsm = (int) CH;
				break;
	
			case (int) CH:
				cstr = (char*) malloc(sizeof(char) * read_id.length() - 1);
				read_id.copy(cstr, read_id.length() - 2, 0); cstr[read_id.length() - 1] = '\0';
				read_id_1.assign(cstr, read_id.length() - 2); free(cstr);
				
				if(!read_id_1.compare(checkpoint_id_4)) { // B-type Checkpoint
						int digit = read_id.length() - read_id_1.length() - 1;
						char temp[digit + 1]; 
						read_id.copy(temp, digit, 1); temp[digit] = '\0';
						int B_id = char_int(temp); 
						BNum_check += 1; // Accumulate the number of B-type Branch having been parsed
						if(B_id > checkpoint_numbers[task_id - 1].B_ch) {
							cout << "The number of B-type Checkpoints doesn't match the pre-defined numbers" << endl;
							exit(1);
						}
						else B_id -= 1; 
						read_content >> read_int; 
						checkpoint_BlockID[task_id - 1].B_checkpoints.push_back(char_int(read_int.c_str())); 
						read_content >> read_int; 
						cycle_trace[task_id - 1].B_RWCEC_t[B_id][0] = char_int(read_int.c_str()); 
						read_content >> read_int; 
						cycle_trace[task_id - 1].B_RWCEC_t[B_id][1] = char_int(read_int.c_str()); 
						read_content >> read_int; 
						cycle_trace[task_id - 1].B_RWCEC_t[B_id][2] = char_int(read_int.c_str()); 
						read_content >> read_int; 
						cycle_trace[task_id - 1].B_RWCEC_t[B_id][3] = char_int(read_int.c_str()); 
				}
				else if(!read_id_1.compare(checkpoint_id_5)) { // L-type Checkpoint
						int digit = read_id.length() - read_id_1.length() - 1;
						char temp[digit + 1]; 
						read_id.copy(temp, digit, 1); temp[digit] = '\0';
						int L_id = char_int(temp); 
						LNum_check += 1; // Accumulate the number of L-type Branch having been parsed
						if(L_id > checkpoint_numbers[task_id - 1].L_ch) {
							cout << "The number of L-type Checkpoints doesn't match the pre-defined numbers" << endl;
							exit(1);
						}
						else L_id -= 1;
						
						for(int i = 0; i < 2; i++)
						 for(int j = 0; j < 8; j++) {
						   read_content >> read_int; 
						   cycle_trace[task_id - 1].L_RWCEC_t[L_id][i][j] = char_int(read_int.c_str()); 
						 } 
				}
				else if(!read_id_1.compare(checkpoint_id_6)) { // P-type Checkpoint
						int digit = read_id.length() - read_id_1.length() - 1;
						char temp[digit + 1]; 
						read_id.copy(temp, digit, 1); temp[digit] = '\0';
						int P_id = char_int(temp); 
						PNum_check += 1; // Accumulate the number of P-type Branch having been parsed
						if(P_id > checkpoint_numbers[task_id - 1].P_ch) {
							cout << "The number of P-type Checkpoints doesn't match the pre-defined numbers" << endl;
							exit(1);
						}
						else P_id -= 1; 
						read_content >> read_int; 
						checkpoint_BlockID[task_id - 1].P_checkpoints.push_back(char_int(read_int.c_str())); 
						read_content >> read_int; 
						cycle_trace[task_id - 1].P_RWCEC_t[P_id][0] = char_int(read_int.c_str()); 
						read_content >> read_int; 
						cycle_trace[task_id - 1].P_RWCEC_t[P_id][1] = char_int(read_int.c_str()); 
						read_content >> read_int;  
						cycle_trace[task_id - 1].P_RWCEC_t[P_id][2] = char_int(read_int.c_str()); 
				}
				else {
					cout << "Wrong syntax(1) \"" << read_id << "\"" << endl;
					cout << "Wrong syntax(2) \"" << read_id_1 << "\"" << endl;
					cout << "Wrong format for defining the B-/L-/P-type Checkpoint(s)" << endl;
					exit(1);
				}
				fsm = (
				       TaskNum_check != tasks_num && 
		                       BNum_check == checkpoint_numbers[task_id - 1].B_ch &&
				       LNum_check == checkpoint_numbers[task_id - 1].L_ch &&
				       PNum_check == checkpoint_numbers[task_id - 1].P_ch  
				      ) ? (int) TSK_ID : fsm;
				break;
			
			default:
				cout << "The Checkpoint-Defined file with wrong format was read" << endl;
				exit(1);
				break;
		}
	}
	for(int cnt = 0; cnt < tasks_num; cnt++) {
		cout << "========================================" << endl;
		cout << endl << "Task_" << cnt + 1 << endl; 
		cout << "B-type Checkpoint:" << endl;
		if(checkpoint_numbers[cnt].B_ch == 0) cout << "    No B-type Checkpoint is inserted in this Task" << endl;
		else {
		 for(int i = 0; i < checkpoint_numbers[cnt].B_ch; i++) 
			cout << "B" << i + 1 << "(" << checkpoint_BlockID[cnt].B_checkpoints[i] << "): " 
						     << cycle_trace[cnt].B_RWCEC_t[i][0] << " " 
						     << cycle_trace[cnt].B_RWCEC_t[i][1] << " "
		 				     << cycle_trace[cnt].B_RWCEC_t[i][2] << " "
		 				     << cycle_trace[cnt].B_RWCEC_t[i][3] << endl;
		}
		cout << "L-type Checkpoint:" << endl;
		if(checkpoint_numbers[cnt].L_ch == 0) cout << "    No L-type Checkpoint is inserted in this Task" << endl;
		else {
		 for(int i = 0; i < checkpoint_numbers[cnt].L_ch; i++)		
		  for(int j = 0; j < 2; j++)
		   for(int k = 0; k < 8; k++) 
		     cout << "L(" << i + 1 << "," << j << "," << k << "): " << cycle_trace[cnt].L_RWCEC_t[i][j][k] << endl;
		}
		cout << "P-type Checkpoint:" << endl;
		if(checkpoint_numbers[cnt].P_ch == 0) cout << "    No P-type Checkpoint is inserted in this Task" << endl;
		else {
		 for(int i = 0; i < checkpoint_numbers[cnt].P_ch; i++) 
			cout << "P" << i + 1 << "("  << checkpoint_BlockID[cnt].P_checkpoints[i] << "): "
						     << cycle_trace[cnt].P_RWCEC_t[i][0] << " " 
						     << cycle_trace[cnt].P_RWCEC_t[i][1] << " "
						     << cycle_trace[cnt].P_RWCEC_t[i][2] << endl;
		}
	}
	cout << "========================================" << endl;
}

int char_int(const char *in) {
	int temp = 0, cnt = 0;
	
	do {
		temp *= 10; 
		temp += (in[cnt] - 48);
		cnt += 1;
	} while(in[cnt]);

	return temp;
}

float char_float(const char *in) {
	float temp = 0.0;
	int cnt = 0, digit = 1, mul, i;
	
	do {
		temp *= 10; 
		temp += (in[cnt] - 48);
		cnt += 1;
	} while(in[cnt] && in[cnt] != 0x2E);
	
	if(in[cnt] == 0x2E) {
		cnt += 1;
		do {
			for(i = 0, mul = 1; i < digit; i++) mul *= 10; 
			temp += ((float) (in[cnt] - 48) / mul);
			cnt += 1; digit += 1;
		} while(in[cnt]);
	}

	return temp;
}
