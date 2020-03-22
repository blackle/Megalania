#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct MinMaxHeap_struct MinMaxHeap;
typedef int (*MinMaxHeapComparator)(void* comparator_data, unsigned a, unsigned b);

MinMaxHeap* min_max_heap_new(size_t size, MinMaxHeapComparator comparator, void* comparator_data);
void min_max_heap_free(MinMaxHeap* heap);

size_t min_max_heap_count(const MinMaxHeap* heap);

bool min_max_heap_insert(MinMaxHeap* heap, unsigned value);

bool min_max_heap_minimum(MinMaxHeap* heap, unsigned* value);
bool min_max_heap_maximum(MinMaxHeap* heap, unsigned* value);

bool min_max_heap_remove_minimum(MinMaxHeap* heap);
bool min_max_heap_remove_maximum(MinMaxHeap* heap);
