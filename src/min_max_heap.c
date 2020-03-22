#include "min_max_heap.h"
#include <stdlib.h>
#include <stdio.h>

struct MinMaxHeap_struct {
	unsigned* store;
	size_t store_size;
	size_t count;
	MinMaxHeapComparator comparator;
	void* comparator_data;
};

MinMaxHeap* min_max_heap_new(size_t size, MinMaxHeapComparator comparator, void* comparator_data)
{
	MinMaxHeap* heap = malloc(sizeof(MinMaxHeap));
	if (heap == NULL) {
		fprintf(stderr, "Could not allocate memory for MinMaxHeap.\n");
		return NULL;
	}
	heap->store = malloc(sizeof(unsigned) * size);
	if (heap->store == NULL) {
		fprintf(stderr, "Could not allocate memory for MinMaxHeap store.\n");
		free(heap);
		return NULL;
	}
	heap->store_size = size;
	heap->count = 0;
	heap->comparator = comparator;
	heap->comparator_data = comparator_data;
	return heap;
}

void min_max_heap_free(MinMaxHeap* heap)
{
	if (heap == NULL) {
		return;
	}
	free(heap->store);
	free(heap);
}

size_t min_max_heap_count(const MinMaxHeap* heap)
{
	return heap->count;
}

bool min_max_heap_insert(MinMaxHeap* heap, unsigned value)
{
	(void)heap;(void)value;
	return false;
}

bool min_max_heap_minimum(MinMaxHeap* heap, unsigned* value)
{
	(void)heap;(void)value;
	return false;

}

bool min_max_heap_maximum(MinMaxHeap* heap, unsigned* value)
{
	(void)heap;(void)value;
	return false;

}

bool min_max_heap_remove_minimum(MinMaxHeap* heap)
{
	(void)heap;
	return false;
}

bool min_max_heap_remove_maximum(MinMaxHeap* heap)
{
	(void)heap;
	return false;
}