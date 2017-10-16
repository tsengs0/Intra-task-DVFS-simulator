#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace std;

enum {
	ATTRIBUTES_ITEM = 0x01, // "{}" for task attributes
	TASK_ATTRIBUTES = 0x02, // For contents of task attributes
	SUCCESSORS_ITEM = 0x03, // "{}" for its successors
	SUCCESSORS	= 0x04  // For contents of its successors
};

enum {
	TASK_ID = 0x05,
	WCEC    = 0x06,
	ACEC    = 0x07,
	BCEC    = 0x08
};

typedef vector<char> char_str;
typedef vector<int> array_int_element;
vector<char_str> task_id;
char_str task_id_temp;
vector<char_str> wcec, acec, bcec;
char_str wcec_temp, acec_temp, bcec_temp;

vector<array_int_element> task_succ;
vector<int> succ;
int succ_int_temp;
 
int main(int argc, char **argv)
{
	char *file_name = argv[1];
	FILE *fp;
	unsigned int c, i, j, k;
	unsigned char fsm_state = (unsigned char) ATTRIBUTES_ITEM;
	unsigned char fsm_state_internal = (unsigned char) TASK_ID;
	
	succ_int_temp = 0;

	fp = fopen(file_name, "r");
	while(1) {
		c = fgetc(fp); 
		if( feof(fp) ) break;
		else  {
			if(c == 0x0A || c == ' ') { while( c == 0x0A || c == ' ' ) c = fgetc(fp);  }
			printf("c = %c state:0x%02X 0x%02X\r\n", c, (unsigned char) fsm_state, (unsigned char) fsm_state_internal);	
			if( c == '{' || c == '}' ) {		
				switch(c)
				{
					case '{': // Identifying 1) Task Attributes or 2) Its successors list
						fsm_state = (fsm_state == (unsigned char) ATTRIBUTES_ITEM) ? (unsigned char) TASK_ATTRIBUTES : 
							    (fsm_state == (unsigned char) SUCCESSORS_ITEM) ? (unsigned char) SUCCESSORS      :
											                             fsm_state;
						break;
				
					case '}': 
						c = fgetc(fp);
						if(c == ',') 
							fsm_state = (unsigned char) SUCCESSORS_ITEM;
						else if(c == ';') {
							fsm_state = (unsigned char) ATTRIBUTES_ITEM;
							fsm_state_internal = (unsigned char) TASK_ID;
							wcec.push_back(wcec_temp); wcec_temp.clear();
							acec.push_back(acec_temp); acec_temp.clear();
							bcec.push_back(bcec_temp); bcec_temp.clear();
							
							// After last element, clearing is still needed
							succ.push_back(succ_int_temp); succ_int_temp = 0; 
							
							task_succ.push_back(succ); succ.clear();
							cout << endl;
						}
						else 
							cout << "Wrong Format, expecting either\",\" or \";\"" << endl;
						break;
	
					default:
						break;
				}
			}
			else {
				if(fsm_state == (unsigned char) TASK_ATTRIBUTES && fsm_state_internal == (unsigned char) TASK_ID) {
					if(c != ',') task_id_temp.push_back(c);
					else { task_id.push_back(task_id_temp); task_id_temp.clear(); fsm_state_internal += 0x01; } 
				}
				else if(fsm_state == (unsigned char) TASK_ATTRIBUTES && fsm_state_internal != (unsigned char) TASK_ID) {
					if     (fsm_state_internal == (unsigned char) WCEC && c != ',') wcec_temp.push_back(c);
					else if(fsm_state_internal == (unsigned char) ACEC && c != ',') acec_temp.push_back(c);
					else if(fsm_state_internal == (unsigned char) BCEC && c != '}') bcec_temp.push_back(c);
					else fsm_state_internal += 0x01; 
											   
				}
				else { // fsm_state = SUCCESSORS
					if(c != ',') {
						succ_int_temp = succ_int_temp * 10;
						succ_int_temp = (c != '#') ? succ_int_temp + ( c - 48 ) : 0;		
					}
					else { // c = ','
						succ.push_back(succ_int_temp);
						succ_int_temp = 0;
					}
				}
			}
		}
	}
	fclose(fp);

	for(i = 0; i < task_id.size(); i++) {
		for(j=0;j<task_id[i].size();j++)
			cout << task_id[i][j];
		cout << endl;
	
		cout << "\tWCEC: ";
		for(j=0;j<wcec[i].size();j++)	
			cout << wcec[i][j];
		cout << endl;

		cout << "\tACEC: ";
		for(j=0;j<acec[i].size();j++)	
			cout << acec[i][j];
		cout << endl;


		cout << "\tBCEC: ";
		for(j=0;j<bcec[i].size();j++)	
			cout << bcec[i][j];
		cout << endl;

		cout << "\tsuccessors: {";
		for(j = 0; j < task_succ[i].size(); j++) {
			cout << task_succ[i][j];
			if(j != task_succ[i].size() - 1) cout << ",";
		}
		cout << "}" << endl << endl;
	}
	
	return 0;
}
