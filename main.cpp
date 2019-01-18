#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;
#define SIM_FREQ_NUM 2
#define tasks_num 5

double *freq;
double freq_sim_1[tasks_num] = {1000.0, 1000.0, 1000.0, 1000.0, 1000.0};
double freq_sim_2[tasks_num] = {1000.0, 800.0, 720.0, 300.0, 100.0};

int main()
{
freq = freq_sim_1;
	cout << "freq sim 1:" << endl;
	//freq = freq_sim_1;
	for(int i = 0; i < tasks_num; i++) printf("freq[%d][%d]: %f MHz\r\n", 0, i, freq[i]);
	
	cout << "freq sim 2:" << endl;
	//freq = freq_sim_2;	
	for(int i = 0; i < tasks_num; i++) printf("freq[%d][%d]: %f MHz\r\n", 0, i, freq[i]);

	return 0;
}
