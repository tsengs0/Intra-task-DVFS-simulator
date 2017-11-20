#include <iostream>
#include "pattern_gen.h"

int main(int argc, char **argv)
{
	testbench tb(5, (unsigned char) 1, (unsigned char) 2);
	tb.param_show();

	return 0;
}
