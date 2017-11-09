#include "pattern_gen.h"

using std::cout;
using std::endl;
using std::vector;

typedef vector<int> ExePath_case;
typedef vector<ExePath_case> ExePath_set;
/**
  * @brief Randomly generate an execution path as test pattern(s) for indeicate Control Flow Graph 

  * @param CFG_path: target Control Flow Graph, i.e., target task
  * @param pattern_num: the number of test patterns which are required
  * @param checkpointLabel: reference for searching Loop Entry's Basic Block
  * @param TestPattern_inout: given memory space for filling generated set of test pattern
**/
void rand_ExePath_gen(
	vector<Basic_block> &CFG_path, 
	int &pattern_num, 
	checkpoints_label &checkpointLabel,
	void *TestPattern_inout,
	void *Lookahead_LoopIteration_inout
)
{
	ExePath_case path_temp;
	ExePath_set &TestPattern_set = *(ExePath_set*) TestPattern_inout;
	int **ActLoop_iteration = (int**) Lookahead_LoopIteration_inout;
	int ActLoop_iteration_cnt[checkpointLabel.P_checkpoints.size()][pattern_num];
	int LoopID, cur_NodeID, succ_NodeID;
	int cnt = pattern_num;
	while(cnt != 0) {
		cur_NodeID = CFG_path.front().get_index(); // Let any test pattern of execution path start from start node of its CFG
		path_temp.push_back(cur_NodeID); // Add current node's ID into current test pattern
		for(int i = 0; i < checkpointLabel.P_loop_bound.size(); i++) { // Randomly give actual loop iteration(s) for all loops
			ActLoop_iteration[i][pattern_num - cnt] = rand() % checkpointLabel.P_loop_bound[i];
			ActLoop_iteration_cnt[i][pattern_num - cnt] = ActLoop_iteration[i][pattern_num - cnt];
			cout << "ActLoop_iteration[" << i << "][" << pattern_num - cnt << "]" << endl;
			cout << "[" << i << "][" << pattern_num - cnt << "]: " << ActLoop_iteration[i][pattern_num - cnt] 
			     << "(cnt: " << ActLoop_iteration_cnt[i][pattern_num - cnt] << ",Loop Bound: " 
			     <<  checkpointLabel.P_loop_bound[i] << ")" << endl; 
		}
		do {
			LoopID = isLoopEntry(cur_NodeID, CFG_path, checkpointLabel); 
			/*Basic_block *curTemp = &CFG_path[cur_NodeID - 1];*/
			if( // If current node is a branch
			   CFG_path[cur_NodeID - 1].succ.size() != 0 && 
			   LoopID == -1
			)  {
				int i = CFG_path[cur_NodeID - 1].succ.size(); // Get the number of current node's successors
				// Randomly choose a branch among all current node's successors
				// Case 1: loop entry
				// Case 2: Multiple successive execution paths, none of them is loop entry 
				int j = rand() % i; // Randomly choose one of branches 
				succ_NodeID = CFG_path[cur_NodeID - 1].succ[j]; 
			}
			else if( // If current node is loop entry, conducting a recursion of randomly generating sub-execution path inside every iteration, 
				 // in addition, avoiding such recursion being run over given "actual loop iteration(s)" 
				
				// Make sure there is only one successive path from current node (only reagarding the notion of for-loop and while-loop)
				CFG_path[cur_NodeID - 1].succ.size() != 0 &&   
			   	LoopID != -1
			) {
				// Run out the budget of "Randomly generated actual loop iteration(s), so forcing to leave the loop"
				if(ActLoop_iteration_cnt[LoopID][pattern_num - cnt] <= 0) { 
					int i = CFG_path[cur_NodeID - 1].succ.size() - 1; // Assume that only last index branch leaves loop
					succ_NodeID = CFG_path[cur_NodeID - 1].succ[i];
				}
				else { // Still can branch into the loop
					ActLoop_iteration_cnt[LoopID][pattern_num - cnt] -= 1;
					int i = CFG_path[cur_NodeID - 1].succ.size() - 1; // Assume that only last index branch leaves loop
					int j = rand() % i; // Thus, only select a branch among index_1 to its index_max
					succ_NodeID = CFG_path[cur_NodeID - 1].succ[j];
				}
			}
			else { // For exception case that current node is neither branch nor Loop Entry
				// Just go down to next Basic Block (note that there suppose be only one successor)
				succ_NodeID = CFG_path[cur_NodeID - 1].succ[0]; 
			}
			cur_NodeID = succ_NodeID;
			path_temp.push_back(cur_NodeID);
		} while(CFG_path[cur_NodeID - 1].get_index() != CFG_path.back().get_index());
		
		TestPattern_set.push_back(path_temp); // Finish generating a test pattern (one execution path)
		vector<int>().swap(path_temp); // Free memory space
		cnt--; // One pattern-number budget has been used
	}
}

/**
  * @brief To check whether current Basic Block is a loop entry or not 

  * @param cur_NodeID: match the Loop Entry's owner according to passed Basic Block ID 
  * @param CFG_path: target Control Flow Graph, i.e., target task
  * @param checkpointLabel: reference for searching Loop Entry's Basic Block
  * @return return -1 if current Basic Block is not a Loop Entry otherwise returning its corresponding LoopID
**/
int isLoopEntry(int cur_NodeID, vector<Basic_block> &CFG_path, checkpoints_label &checkpointLabel)
{
	int i;
	// Identify currently reaching node belongs to which loop entry (the label of P-checkpoint)
	for(i = 0; checkpointLabel.P_loop_entry[i] != cur_NodeID; i++) {
	  if(i >= checkpointLabel.P_loop_entry.size()) {
	    //cout << "Couldn't find out Basic Block " << cur_NodeID << "'s Loop Entry index" << endl;
	    return -1; 
	  }
	}
	return i; 
}
