#include "parser.h"

int main(int argc, char **argv)
{
	RWCEC_Trace_in *cycle_trace;
	checkpoint_num *checkpoint_num_t;
	checkpoints_t *checkpoint_BlockID;

	Parser parsing;	
	parsing.checkpoint_in(
		3, // Task Numbers
		cycle_trace,
		checkpoint_num_t,
		checkpoint_BlockID		
	);


	return 0;
}
