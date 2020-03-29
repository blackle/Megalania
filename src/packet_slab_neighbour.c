#include "packet_slab_neighbour.h"
#include "lzma_state.h"
#include "lzma_packet_encoder.h"
#include "perplexity_encoder.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void packet_slab_neighbour_new(PacketSlabNeighbour* neighbour, PacketSlab* slab, const uint8_t* data, size_t data_size)
{
	neighbour->data = data;
	neighbour->data_size = data_size;
	neighbour->slab = slab;
	neighbour->perplexity = 0;
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

static bool pick_random_next_packet_from_top_k(const LZMAState* lzma_state, TopKPacketFinder* packet_finder, LZMAPacket* packets, bool best)
{
	LZMAPacket* next_packet = &packets[lzma_state->position];
	top_k_packet_finder_find(packet_finder, lzma_state, packets);
	size_t count = top_k_packet_finder_count(packet_finder);
	if (count == 0) {
		return false;
	}
	size_t choice = rand() % count;
	while (top_k_packet_finder_pop(packet_finder, next_packet)) {
		if (choice-- == 0 || best) {
			return true;
		}
	}
	return true;
}

static bool validate_long_rep_packet(const LZMAState* lzma_state, LZMAPacket packet)
{
	assert(packet.type == LONG_REP);
	const uint8_t* rep_start = &lzma_state->data[lzma_state->position - lzma_state->dists[packet.dist] - 1];
	const uint8_t* current = &lzma_state->data[lzma_state->position];
	return memcmp(rep_start, current, packet.len) == 0;
}

static void repair_remaining_packets(PacketSlabNeighbour* neighbour, LZMAState* lzma_state, EncoderInterface* enc, TopKPacketFinder* packet_finder, LZMAPacket* packets)
{
	size_t count = 0;
	while (lzma_state->position < lzma_state->data_size) {
		count++;
		LZMAPacket* packet = &packets[lzma_state->position];
		LZMAPacket old_packet = *packet;

		if (packet->type == SHORT_REP || packet->type == LITERAL) {
			if (lzma_state->data[lzma_state->position] == lzma_state->data[lzma_state->position - lzma_state->dists[0] - 1]) {
				if (count < 4) {
					*packet = short_rep_packet();
				}
			} else {
				*packet = literal_packet();
			}
		}
		if (packet->type == LONG_REP) {
			unsigned dist_index = 0;
			while (!validate_long_rep_packet(lzma_state, *packet) && dist_index < 4) {
				packet->dist = dist_index;
				dist_index++;
			}
			if (!validate_long_rep_packet(lzma_state, *packet)) {
				//we don't have to worry about not having a second packet here because there will always be the literal packet
				pick_random_next_packet_from_top_k(lzma_state, packet_finder, packets, true);
			}
		}

		if (!lzma_packet_cmp(&old_packet, packet)) {
			save_packet_at_position(neighbour, old_packet, lzma_state->position);
		}

		lzma_encode_packet(lzma_state, enc, *packet);
	}
}

bool packet_slab_neighbour_generate(PacketSlabNeighbour* neighbour, TopKPacketFinder* packet_finder)
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
	if (!pick_random_next_packet_from_top_k(&lzma_state, packet_finder, packets, false)) {
		return false;
	}
	lzma_encode_packet(&lzma_state, &enc, packets[lzma_state.position]);

	repair_remaining_packets(neighbour, &lzma_state, &enc, packet_finder, packets);
	return true;
}

void packet_slab_neighbour_undo(PacketSlabNeighbour* neighbour)
{
	packet_slab_undo_stack_apply(&neighbour->undo_stack, neighbour->slab);
}

size_t packet_slab_neighbour_undo_count(const PacketSlabNeighbour* neighbour)
{
	return packet_slab_undo_stack_count(&neighbour->undo_stack);
}
