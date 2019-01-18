#include <iostream>
#include <cstring>

using namespace std;

int main(int argc, char **argv)
{
	int info[3][3] = {
		{1, 2, 3},
		{4, 5, 6},
		{7, 8, 9}
	};
	int info_1[3][3];

	memcpy(info_1, (int[3][3]) {{1,2,3},{4,5,6},{7,8,9}}, 3*3*sizeof(int));

	for(int i = 0; i < 3; i++)
	 for(int j = 0; j < 3; j++) 
	  cout << "info[" << i << "][" << j << "]: " << info_1[i][j] << endl; 	

	return 0;
}
