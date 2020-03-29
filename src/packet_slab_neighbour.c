#include "packet_slab_neighbour.h"
#include "lzma_state.h"
#include "lzma_packet_encoder.h"
#include "perplexity_encoder.h"
#include <stdlib.h>

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

static void encode_to_packet_number(PacketSlabNeighbour* neighbour, LZMAState* state, EncoderInterface* enc, LZMAPacket* packets, size_t target)
{
	size_t count = 0;
	while (state->position < neighbour->data_size) {
		if (count == target) {
			break;
		}
		count++;
		lzma_encode_packet(state, enc, packets[state->position]);
	}
}

void packet_slab_neighbour_generate(PacketSlabNeighbour* neighbour)
{
	LZMAPacket* packets = packet_slab_packets(neighbour->slab);

	LZMAState state;
	lzma_state_init(&state, neighbour->data, neighbour->data_size);
	EncoderInterface enc;
	perplexity_encoder_new(&enc, &neighbour->perplexity);

	size_t packet_count = packet_slab_count(neighbour->slab);
	size_t mutation_target = rand() % packet_count;
	encode_to_packet_number(neighbour, &state, &enc, packets, mutation_target);

	//todo: pick new packet randomly from the top N

	//todo: fix downstream packets
}

void packet_slab_neighbour_undo(PacketSlabNeighbour* neighbour)
{
	packet_slab_undo_stack_apply(&neighbour->undo_stack, neighbour->slab);
}
