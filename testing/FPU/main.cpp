#include <iostream>
#include <cstdio>
#include <cstdlib>

using std::cout;
using std::endl;

int main(int argc, char **argv)
{
	float a, b;

	a = 1.522228;
	b = 1.522224;

	cout << "a = " << a << endl
	     << "b = " << b << endl;
	
	printf("a = %f, b = %f\r\n", a, b);

	return 0;
}
