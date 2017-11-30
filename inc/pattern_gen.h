#ifndef __PATTERN_GEN_H
#define __PATTERN_GEN_H
#include <iostream>
#include <vector>
#include "cfg_info.h"
#include "checkpoint_info.h"

void rand_ExePath_gen(
	std::vector<Basic_block> &CFG_path, 
	int pattern_num, 
	checkpoints_label &checkpointLabel, 
	void *TestPattern_inout,
	void *Lookahead_LoopIteration1_inout,
	void *Lookahead_LoopIteration2_inout,
	void *Lookahead_LoopIteration3_inout
);
	
int isLoopEntry(int cur_NodeID, std::vector<Basic_block> &CFG_path, checkpoints_label &checkpointLabel);

#endif // __PATTERN_GEN_H

