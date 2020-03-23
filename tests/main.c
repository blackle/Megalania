#include "max_heap_test.h"
#include "substring_enumerator_test.h"

int main(int argc, char** argv)
{
	(void) argc; (void) argv;

	substring_enumerator_test();
	max_heap_test();

	return 0;
}