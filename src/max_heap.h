#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct MaxHeap_struct MaxHeap;
typedef int (*MaxHeapComparator)(void* comparator_data, unsigned a, unsigned b);

MaxHeap* max_heap_new(size_t size, MaxHeapComparator comparator, void* comparator_data);
void max_heap_free(MaxHeap* heap);

size_t max_heap_count(const MaxHeap* heap);

bool max_heap_insert(MaxHeap* heap, unsigned value);

bool max_heap_minimum(MaxHeap* heap, unsigned* value);
bool max_heap_maximum(MaxHeap* heap, unsigned* value);

bool max_heap_remove_minimum(MaxHeap* heap);
bool max_heap_remove_maximum(MaxHeap* heap);
