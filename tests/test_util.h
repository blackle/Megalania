#include <stdio.h>

#define PRETTY_PRINT_TEST_NAME() \
{ \
	printf("=== %s ===\n", __FUNCTION__); \
}

#define TEST_ASSERT(test, msg) \
{ \
	if(!(test)) { \
		fprintf(stderr, msg); \
		fprintf(stderr, "\n"); \
		return -1; \
	} \
}

#define TEST_ASSERT_EQ(got, expect, msg) \
{ \
	typeof(got) __GOT_STOR = got; \
	typeof(expect) __EXPECT_STOR = expect; \
	if(__GOT_STOR != __EXPECT_STOR) { \
		fprintf(stderr, msg, __EXPECT_STOR, __GOT_STOR); \
		fprintf(stderr, "\n"); \
		return -1; \
	} \
}
