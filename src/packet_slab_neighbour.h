#pragma once
#include "packet_slab.h"
#include "packet_slab_undo_stack.h"
#include "top_k_packet_finder.h"
#include <stdbool.h>

typedef struct {
	LZMAState init_state;
	PacketSlab* slab;

	uint64_t perplexity;
	PacketSlabUndoStack undo_stack;
} PacketSlabNeighbour;

void packet_slab_neighbour_new(PacketSlabNeighbour* neighbour, PacketSlab* slab, LZMAState init_state);
void packet_slab_neighbour_free(PacketSlabNeighbour* neighbour);

bool packet_slab_neighbour_generate(PacketSlabNeighbour* neighbour, TopKPacketFinder* packet_finder);
void packet_slab_neighbour_undo(PacketSlabNeighbour* neighbour);
size_t packet_slab_neighbour_undo_count(const PacketSlabNeighbour* neighbour);
