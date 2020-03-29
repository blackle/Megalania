#pragma once
#include "lzma_packet.h"
#include "lzma_state.h"
#include "packet_enumerator.h"
#include <stdbool.h>

typedef struct TopKPacketFinder_struct TopKPacketFinder;

TopKPacketFinder* top_k_packet_finder_new(size_t size, const PacketEnumerator* packet_enumerator);
void top_k_packet_finder_free(TopKPacketFinder* finder);

size_t top_k_packet_finder_count(const TopKPacketFinder* finder);
void top_k_packet_finder_find(TopKPacketFinder* finder, const LZMAState* lzma_state);

bool top_k_packet_finder_pop(TopKPacketFinder* finder, LZMAPacket* packet);
