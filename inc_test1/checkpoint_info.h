#ifndef __CHECKPOINT_INFO_H
#define __CHECKPOINT_INFO_H

typedef vector<int> B_checkpoints_label;
typedef vector< vector<int> > L_checkpoints_label;
typedef vector<int> P_checkpoints_label;
typedef vector<int> L_loop_bound_t;
typedef vector<int> P_loop_bound_t;
typedef struct Checkpoints_Label {
	B_checkpoints_label B_checkpoints;
	L_checkpoints_label L_checkpoints;
	P_checkpoints_label P_checkpoints;
	L_loop_bound_t L_loop_bound; // Recording the number of iteration every loop nest have run so far
	P_loop_bound_t P_loop_bound;
} checkpoints_label;

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