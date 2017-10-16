#ifndef __PARSER_H
#define __PARSER_H

#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <vector>
#include "../inc_test1/checkpoint_info.h"
#include "../inc_test1/cfg_info_test1.h"

void parse_checkpoint(void);
int char_int(const char *in);
float char_float(const char *in);

//====================================================//
// Finte State Machine for Checkpoint File Parser
enum {
	TSK_NUM = 0,
	TSK_ID  = 1,
	CH_NUM  = 2,
	CH      = 3,
	END	= 4	
};
//===================================================//

class Parser {
	private:
		
	public:
		Parser(void);
		~Parser(void);
	
		void checkpoint_in(int tsk_num_in, RWCEC_Trace_in *cycle_trace, checkpoint_num *checkpoint_numbers, checkpoints_t *checkpoint_BlockID);
}; 



#endif // __PARSER_H 
