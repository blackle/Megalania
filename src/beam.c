#include "beam.h"
#include "max_heap.h"
#include <string.h>
#include <stdlib.h>

struct Beam_struct {
	BeamEntry* entries;
	size_t beam_size;
	MaxHeap* heap;
};

static int sign(float x)
{
	if (x < 0.f) return -1;
	if (x > 0.f) return 1;
	return 0;
}

static int beam_entry_comparator(void* user_data, unsigned a, unsigned b)
{
	Beam* beam = (Beam*) user_data;
	//todo: assert
	BeamEntry* entry_a = &beam->entries[a];
	BeamEntry* entry_b = &beam->entries[b];
	return sign(entry_a->compression_ratio - entry_b->compression_ratio);
}

Beam* beam_new(size_t beam_size)
{
	Beam* beam = malloc(sizeof(Beam));
	if (beam == NULL) {
		return NULL;
	}
	beam->beam_size = beam_size;
	beam->entries = malloc(sizeof(BeamEntry) * beam_size);
	if (beam->entries == NULL) {
		free(beam);
		return NULL;
	}
	beam->heap = max_heap_new(beam_size, beam_entry_comparator, beam);
	if (beam->heap == NULL) {
		free(beam->entries);
		free(beam);
		return NULL;
	}
	return beam;
}

void beam_free(Beam* beam)
{
	max_heap_free(beam->heap);
	free(beam->entries);
	free(beam);
}

size_t beam_count(const Beam* beam)
{
	return max_heap_count(beam->heap);
}

void beam_clear(Beam* beam)
{
	max_heap_clear(beam->heap);
}

void beam_insert(Beam* beam, const BeamEntry* entry)
{
	size_t count = beam_count(beam);

	//if there is room, just insert
	if (count < beam->beam_size) {
		size_t pos = count++;
		memcpy(&beam->entries[pos], entry, sizeof(BeamEntry));
		max_heap_insert(beam->heap, pos);
		return;
	}

	//otherwise, we must compare with the maximum and replace if needed
	unsigned maximum;
	max_heap_maximum(beam->heap, &maximum); //todo: assert?
	if (entry->compression_ratio < beam->entries[maximum].compression_ratio) {
		memcpy(&beam->entries[maximum], entry, sizeof(BeamEntry));
		max_heap_update_maximum(beam->heap);
	}
}

const BeamEntry* beam_entries(const Beam* beam)
{
	return beam->entries;
}
