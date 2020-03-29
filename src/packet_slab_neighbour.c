#include "packet_slab_neighbour.h"
#include "lzma_state.h"
#include "lzma_packet_encoder.h"
#include "perplexity_encoder.h"
#include <stdlib.h>
#include <string.h>

void packet_slab_neighbour_new(PacketSlabNeighbour* neighbour, PacketSlab* slab, const uint8_t* data, size_t data_size)
{
	neighbour->data = data;
	neighbour->data_size = data_size;
	neighbour->slab = slab;
	neighbour->perplexity = 0.f;
	packet_slab_undo_stack_new(&neighbour->undo_stack);
}

void packet_slab_neighbour_free(PacketSlabNeighbour* neighbour)
{
	packet_slab_undo_stack_free(&neighbour->undo_stack);
}

static void encode_to_packet_number(LZMAState* lzma_state, EncoderInterface* enc, LZMAPacket* packets, size_t target)
{
	size_t count = 0;
	while (lzma_state->position < lzma_state->data_size) {
		if (count == target) {
			break;
		}
		count++;
		lzma_encode_packet(lzma_state, enc, packets[lzma_state->position]);
	}
}

static void save_packet_at_position(PacketSlabNeighbour* neighbour, LZMAPacket packet, size_t position)
{
	PacketSlabUndo undo = { .position = position, .old_packet = packet };
	packet_slab_undo_stack_insert(&neighbour->undo_stack, undo);
}

#define TOP_PACKET_CHANCE 4
static void pick_random_next_packet_from_top_k(const LZMAState* lzma_state, TopKPacketFinder* packet_finder, LZMAPacket* packets)
{
	top_k_packet_finder_find(packet_finder, lzma_state);
	LZMAPacket* next_packet = &packets[lzma_state->position];
	while (top_k_packet_finder_pop(packet_finder, next_packet)) {
		if (rand() % TOP_PACKET_CHANCE > 0) {
			return;
		}
	}
}

static void repair_remaining_packets(PacketSlabNeighbour* neighbour, LZMAState* lzma_state, EncoderInterface* enc, TopKPacketFinder* packet_finder, LZMAPacket* packets)
{
	while (lzma_state->position < lzma_state->data_size) {
		LZMAPacket* packet = &packets[lzma_state->position];
		LZMAPacket old_packet = *packet;

		if (packet->type == SHORT_REP) {
			if (lzma_state->data[lzma_state->position] != lzma_state->data[lzma_state->position - lzma_state->dists[0] - 1]) {
				*packet = literal_packet();
			}
		}
		if (packet->type == LONG_REP) {
			//todo: this can be better, iterate over the dists to see if any are valid
			pick_random_next_packet_from_top_k(lzma_state, packet_finder, packets);
		}

		if (memcmp(&old_packet, packet, sizeof(LZMAPacket)) != 0) {
			save_packet_at_position(neighbour, old_packet, lzma_state->position);
		}

		lzma_encode_packet(lzma_state, enc, *packet);
	}
}

void packet_slab_neighbour_generate(PacketSlabNeighbour* neighbour, TopKPacketFinder* packet_finder)
{
	LZMAPacket* packets = packet_slab_packets(neighbour->slab);

	LZMAState lzma_state;
	lzma_state_init(&lzma_state, neighbour->data, neighbour->data_size);
	EncoderInterface enc;
	perplexity_encoder_new(&enc, &neighbour->perplexity);

	size_t packet_count = packet_slab_count(neighbour->slab);
	size_t mutation_target = rand() % packet_count;

	encode_to_packet_number(&lzma_state, &enc, packets, mutation_target);

	save_packet_at_position(neighbour, packets[lzma_state.position], lzma_state.position);
	pick_random_next_packet_from_top_k(&lzma_state, packet_finder, packets);
	lzma_encode_packet(&lzma_state, &enc, packets[lzma_state.position]);

	repair_remaining_packets(neighbour, &lzma_state, &enc, packet_finder, packets);
}

void packet_slab_neighbour_undo(PacketSlabNeighbour* neighbour)
{
	packet_slab_undo_stack_apply(&neighbour->undo_stack, neighbour->slab);
}
