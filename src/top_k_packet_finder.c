#include "top_k_packet_finder.h"
#include "perplexity_encoder.h"
#include "lzma_packet_encoder.h"
#include "max_heap.h"
#include <stdlib.h>

typedef struct {
	LZMAPacket packet;
	float cost;
} TopKEntry;

struct TopKPacketFinder_struct {
	size_t size;
	TopKEntry* entries;
	MaxHeap* heap;
	const PacketEnumerator* packet_enumerator;
};

static int sign(float x)
{
	if (x < 0.f) return -1;
	if (x > 0.f) return 1;
	return 0;
}

static int top_k_entry_comparator(void* user_data, unsigned a, unsigned b)
{
	TopKPacketFinder* finder = (TopKPacketFinder*) user_data;
	//todo: assert
	TopKEntry* entry_a = &finder->entries[a];
	TopKEntry* entry_b = &finder->entries[b];
	return sign(entry_a->cost - entry_b->cost);
}

TopKPacketFinder* top_k_packet_finder_new(size_t size, const PacketEnumerator* packet_enumerator)
{
	TopKPacketFinder* finder = malloc(sizeof(TopKPacketFinder));
	if (finder == NULL) {
		return NULL;
	}
	finder->size = size;
	finder->packet_enumerator = packet_enumerator;
	finder->entries = malloc(sizeof(TopKEntry) * size);
	if (finder->entries == NULL) {
		free(finder);
		return NULL;
	}
	finder->heap = max_heap_new(size, top_k_entry_comparator, finder);
	if (finder->heap == NULL) {
		free(finder->entries);
		free(finder);
		return NULL;
	}
	return finder;
}

void top_k_packet_finder_free(TopKPacketFinder* finder)
{
	max_heap_free(finder->heap);
	free(finder->entries);
	free(finder);
}

size_t top_k_packet_finder_count(const TopKPacketFinder* finder)
{
	return max_heap_count(finder->heap);
}

static void top_k_entry_finder_insert(TopKPacketFinder* finder, TopKEntry entry)
{
	size_t count = top_k_packet_finder_count(finder);

	//if there is room, just insert
	if (count < finder->size) {
		size_t pos = count++;
		finder->entries[pos] = entry;
		max_heap_insert(finder->heap, pos);
		return;
	}

	//otherwise, we must compare with the maximum and replace if needed
	unsigned maximum = 0;
	max_heap_maximum(finder->heap, &maximum); //todo: assert? ... why?? oh right, so it's in bounds
	if (entry.cost <= finder->entries[maximum].cost) {
		finder->entries[maximum] = entry;
		max_heap_update_maximum(finder->heap);
	}
}

static void top_k_packet_finder_callback(void* user_data, const LZMAState* state, LZMAPacket packet)
{
	TopKPacketFinder* finder = (TopKPacketFinder*) user_data;

	LZMAState new_state = *state;
	EncoderInterface enc;
	uint64_t perplexity = 0;
	perplexity_encoder_new(&enc, &perplexity);
	lzma_encode_packet(&new_state, &enc, packet);

	TopKEntry entry = { .packet = packet, .cost = perplexity / packet.len };
	top_k_entry_finder_insert(finder, entry);
}

void top_k_packet_finder_find(TopKPacketFinder* finder, const LZMAState* lzma_state)
{
	max_heap_clear(finder->heap);
	packet_enumerator_for_each(finder->packet_enumerator, lzma_state, top_k_packet_finder_callback, finder);
}

bool top_k_packet_finder_pop(TopKPacketFinder* finder, LZMAPacket* packet)
{
	unsigned maximum;
	if (!max_heap_maximum(finder->heap, &maximum)) {
		return false;
	}

	*packet = finder->entries[maximum].packet;

	max_heap_remove_maximum(finder->heap);
	return true;
}
