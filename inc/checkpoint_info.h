#ifndef __CHECKPOINT_INFO_H
#define __CHECKPOINT_INFO_H
#include <vector>

typedef std::vector<int> B_checkpoints_label;
typedef std::vector<int> L_checkpoints_label;
typedef std::vector<int> P_checkpoints_label;
typedef std::vector<int> L_loop_bound_t;
typedef std::vector<int> P_loop_bound_t;
typedef std::vector<int> P_loop_entry_t;
typedef struct Checkpoints_Label {
	B_checkpoints_label B_checkpoints;
	L_checkpoints_label L_checkpoints;
	P_checkpoints_label P_checkpoints;
	L_loop_bound_t L_loop_bound; // Recording the number of iteration every loop nest have run so far
	P_loop_bound_t P_loop_bound;
	P_loop_entry_t P_loop_entry;
} checkpoints_label;

typedef int B_type[4];
typedef int L_branch[5];
typedef struct L_Type {
	int loop_bound;
	int rwcec_AfterLoop;
	int branch_num;
	L_branch *branch;
} L_type;
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

// The tuple of mining tablele
typedef struct B_Mining_table {
	int block_id; // Labelling which Basic Block it belongs to
	int n_taken_rwcec; // The remaining worst-case executin cycles from current checkpoint, if the branch instruction was not taken
	int taken_rwcec; // The remaining worst-case executin cycles from current checkpoint, if the branch instruction was taken
	int successors[2];
} B_mining_table_t;

typedef struct L_Mining_table {
	int loop_entry;
	int loop_bound; // Loop Bound
	std::vector<int> block_id; // Labelling whcih Basic Block current branch belongs to 
	std::vector<int> taken_succ; // Labelling which Basic Block Taken-Successor belongs to
	std::vector<int> taken_rwcec; // The worst-case execution cycles from Taken-Successor to current Loop's exit (within an iteration)
	std::vector<int> n_taken_succ; // Labelling which Basic Block Not-Taken-Successor belongs to
	std::vector<int> n_taken_rwcec; // The worst-case execution cycles from Not-Taken-Successors until current Loop's exit (within an iteration)
	int succ_rwcec; // The remaining worst-case execution cycles (RWCECs) after this loop
} L_mining_table_t;

typedef struct P_Mining_table {
	int block_id;   // Labelling which Basic Block it belongs to
	int loop_bound; // Loop Bound
	int iteration_wcec; // The worst-case execution cycles (WCECs) of one loop iteration
	int succ_rwcec; // The remaining worst-case execution cycles (RWCECs) after this loop
} P_mining_table_t;

#endif // __CHECKPOINT_INFO_H
