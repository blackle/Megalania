#pragma once
#include "lzma_packet.h"
#include "lzma_state.h"
#include "packet_enumerator.h"
#include <stdbool.h>

//The Top-K packet finder will iterate all possible next packets given an input LZMAState. It will construct a list of size K of the packets that produce in the highest compression ratio. This list can then be iterated over by calling "pop", which pops off the worst element in the list. the last element popped will be the best element

typedef struct TopKPacketFinder_struct TopKPacketFinder;

TopKPacketFinder* top_k_packet_finder_new(size_t size, const PacketEnumerator* packet_enumerator);
void top_k_packet_finder_free(TopKPacketFinder* finder);

size_t top_k_packet_finder_count(const TopKPacketFinder* finder);
void top_k_packet_finder_find(TopKPacketFinder* finder, const LZMAState* lzma_state, LZMAPacket* next_packets);

bool top_k_packet_finder_pop(TopKPacketFinder* finder, LZMAPacket* packet);
