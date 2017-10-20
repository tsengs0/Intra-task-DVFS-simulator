#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace std;

struct test {
	int a;
	int b;
	int c[2];
};

int main(int argc, char **argv)
{
	float bound;
	/*vector<struct test> test_t;
	int c_t[2] = {3, 4};
	test_t.push_back( {1, 2, 3, 4} );
	cout << test_t[0].a << endl;
	cout << test_t[0].b << endl;
	cout << test_t[0].c[0] << endl;
	cout << test_t[0].c[1] << endl;
	*/
	cout << "Less requirement on the value of alpha" << endl;
	for(int i = 605; i >= 75; i-- ) {
		bound = (float) i / 1000;
		bound = (float) ((bound - 0.075) / 2.345);
		printf("Actual_EC: %d, bound: %f\%\r\n", i, bound * 100);
	}
	cout << endl;
	cout << "High requirement on the value of alpha" << endl;
	for(int i = 75; i <= 605; i++ ) {
		bound = (float) i / 250;
		bound = (float) ((bound - 0.075) / 2.345);
		printf("Actual_EC: %d, bound: %f\%\r\n", i, bound * 100);
	}

	return 0;
}
