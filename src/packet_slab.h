#pragma once
#include "lzma_packet.h"
#include <stddef.h>

typedef struct PacketSlab_struct PacketSlab;

size_t packet_slab_memory_usage(size_t data_size);

PacketSlab* packet_slab_new(size_t data_size);
void packet_slab_free(PacketSlab* slab);

size_t packet_slab_size(const PacketSlab* slab);
LZMAPacket* packet_slab_packets(PacketSlab* slab);
