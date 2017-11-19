#ifndef __PATTERN_GEN_H
#define __PATTERN_GEN_H
#include <iostream>
#include <vector>
#include "cfg_info.h"
#include "checkpoint_info.h"

void rand_ExePath_gen(
	vector<Basic_block> &CFG_path, 
	int pattern_num, 
	checkpoints_label &checkpointLabel, 
	void *TestPattern_inout,
	void *Lookahead_LoopIteration_inout
);
	
int isLoopEntry(int cur_NodeID, vector<Basic_block> &CFG_path, checkpoints_label &checkpointLabel);

#endif // __PATTERN_GEN_H

