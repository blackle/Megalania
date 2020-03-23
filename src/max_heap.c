#include "max_heap.h"
#include <stdlib.h>
#include <stdio.h>

struct MaxHeap_struct {
	unsigned* store;
	size_t store_size;
	size_t count;
	MaxHeapComparator comparator;
	void* comparator_data;
};

MaxHeap* max_heap_new(size_t size, MaxHeapComparator comparator, void* comparator_data)
{
	MaxHeap* heap = malloc(sizeof(MaxHeap));
	if (heap == NULL) {
		fprintf(stderr, "Could not allocate memory for MaxHeap.\n");
		return NULL;
	}
	heap->store = malloc(sizeof(unsigned) * size);
	if (heap->store == NULL) {
		fprintf(stderr, "Could not allocate memory for MaxHeap store.\n");
		free(heap);
		return NULL;
	}
	heap->store_size = size;
	heap->count = 0;
	heap->comparator = comparator;
	heap->comparator_data = comparator_data;
	return heap;
}

void max_heap_free(MaxHeap* heap)
{
	if (heap == NULL) {
		return;
	}
	free(heap->store);
	free(heap);
}

size_t max_heap_count(const MaxHeap* heap)
{
	return heap->count;
}

bool max_heap_insert(MaxHeap* heap, unsigned value)
{
	(void)heap;(void)value;
	return false;
}

bool max_heap_minimum(MaxHeap* heap, unsigned* value)
{
	(void)heap;(void)value;
	return false;

}

bool max_heap_maximum(MaxHeap* heap, unsigned* value)
{
	(void)heap;(void)value;
	return false;

}

bool max_heap_remove_minimum(MaxHeap* heap)
{
	(void)heap;
	return false;
}

bool max_heap_remove_maximum(MaxHeap* heap)
{
	(void)heap;
	return false;
}