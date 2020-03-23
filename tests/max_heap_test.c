#include "max_heap_test.h"
#include "../src/max_heap.h"
#include "test_util.h"
#include <stdio.h>
#include <stdlib.h>

static int plain_comparator(void* comparator_data, unsigned a, unsigned b)
{
	(void) comparator_data;
	return a - b;
}

void swap(unsigned *a, unsigned *b)
{
	unsigned temp = *a;
	*a = *b;
	*b = temp;
}

static void initialize_random_data(unsigned* data, size_t data_size, unsigned seed)
{
	srand(seed);
	for (size_t i = 0; i < data_size; i++) {
		data[i] = i;
	}

	for (size_t i = 0; i < data_size; i++) {
		size_t j = rand() % (i + 1);
		swap(&data[i], &data[j]);
	}
}

static void print_data(unsigned* data, size_t data_size)
{
	for (size_t i = 0; i < data_size; i++) {
		printf("%d ", data[i]);
	}
}

int max_heap_test()
{
	PRETTY_PRINT_TEST_NAME();

	size_t heap_size = 10;
	MaxHeap* heap = max_heap_new(heap_size, plain_comparator, NULL);
	TEST_ASSERT(heap != NULL, "Could not allocate heap!");

	unsigned data[heap_size];
	initialize_random_data(data, heap_size, 666);

	printf("sorting data with heap: ");
	print_data(data, heap_size);
	printf("\n");

	for (size_t i = 0; i < heap_size; i++) {
		TEST_ASSERT(max_heap_insert(heap, data[i]), "Could not insert into heap!");
	}

	TEST_ASSERT_EQ(max_heap_count(heap), heap_size, "Heap wrong size! expected: %ld, got %ld");

	printf("sorted data: ");
	for (size_t i = 0; i < heap_size; i++) {
		unsigned value = 0;
		unsigned expected_value = heap_size - i - 1;
		TEST_ASSERT(max_heap_maximum(heap, &value), "Could not get heap maximum!");
		TEST_ASSERT_EQ(value, expected_value, "Maximum value in heap unexpected! expected: %d, got %d");
		TEST_ASSERT(max_heap_remove_maximum(heap), "Could not pop heap!");
		printf("%d ", value);
	}
	printf("\n");

	max_heap_free(heap);
	return 0;
}
