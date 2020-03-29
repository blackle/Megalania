#pragma once
#include "packet_slab.h"
#include "packet_slab_undo_stack.h"

typedef struct {
	const uint8_t* data;
	size_t data_size;
	PacketSlab* slab;

	float perplexity;
	PacketSlabUndoStack undo_stack;
} PacketSlabNeighbour;

void packet_slab_neighbour_new(PacketSlabNeighbour* neighbour, PacketSlab* slab, const uint8_t* data, size_t data_size);
void packet_slab_neighbour_free(PacketSlabNeighbour* neighbour);

void packet_slab_neighbour_generate(PacketSlabNeighbour* neighbour);
void packet_slab_neighbour_undo(PacketSlabNeighbour* neighbour);
