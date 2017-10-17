#ifndef __CHECKPOINT_INFO_H
#define __CHECKPOINT_INFO_H

typedef vector<int> B_checkpoints_t;
typedef vector< vector<int> > L_checkpoints_t;
typedef vector<int> P_checkpoints_t;
typedef vector<int> L_loop_iteration_t;
typedef vector<int> P_loop_iteration_t;
// The set of various types of Checkpoints
typedef struct Checkpoints {
	B_checkpoints_t B_checkpoints;
	L_checkpoints_t L_checkpoints;
	P_checkpoints_t P_checkpoints;
	L_loop_iteration_t L_loop_iteration; // Recording the number of iteration every loop nest have run so far
	P_loop_iteration_t P_loop_iteration;
} checkpoints_t;

typedef int B_type[4];
typedef int L_type[2][8];
typedef int P_type[3];
typedef struct RWCEC_Trace {
	B_type *B_RWCEC_t;
	L_type *L_RWCEC_t;
	P_type *P_RWCEC_t; 
} RWCEC_Trace_in;

typedef struct Checkpoint_Num {
	int B_ch;
	int L_ch;
	int P_ch;
} checkpoint_num; 

#endif // __CHECKPOINT_INFO_H
