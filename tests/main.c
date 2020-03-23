#include "max_heap_test.h"

int main(int argc, char** argv)
{
	(void) argc; (void) argv;

	int retval = 0;
	retval |= max_heap_test();
	return retval;
}