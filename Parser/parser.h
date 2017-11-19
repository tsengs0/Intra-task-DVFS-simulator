#ifndef __PARSER_H
#define __PARSER_H

#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <vector>
#include "../inc/checkpoint_info.h"
//#include "cfg_info.h"

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
	
		void checkpoint_in(int tsk_num_in, void *cycle_trace_inout, void *checkpoint_numbers_inout, void *checkpoint_BlockID_inout);
}; 



#endif // __PARSER_H 

