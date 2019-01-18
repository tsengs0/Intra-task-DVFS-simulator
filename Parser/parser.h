#ifndef __PARSER_H
#define __PARSER_H

#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <vector>
#include "../inc/checkpoint_info.h"
//#include "cfg_info.h"

typedef std::vector<int> ExePath_case;
typedef std::vector<ExePath_case> ExePath_set;
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
//====================================================//
// Finte State Machine for Test Pattern File Parser
enum {
	SET_SIZE  = 0,
	CASE_ID   = 1,
	CASE_SIZE = 2,
	BB_CASE   = 3
};
//===================================================//

class Parser {
	private:
		
	public:
		Parser(void);
		~Parser(void);
	
		void checkpoint_in(char *checkpoint_file, int tsk_num_in, void *cycle_trace_inout, void *checkpoint_numbers_inout, void *checkpoint_BlockID_inout);
		void TestPattern_in(char *TestPattern_file, int pattern_num_in, void *TestPatternSet_inout);
		void LaIterationPattern_in(char *LaIterationPattern_file, int pattern_num_in, int **LaIterationPatternSet_inout, checkpoints_label PChNumSet_in);
}; 



#endif // __PARSER_H 

