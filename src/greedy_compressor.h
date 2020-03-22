#pragma once
#include "lzma_state.h"
#include "lzma_packet.h"

typedef struct {
	LZMAState* lzma_state;
	float total_perplexity;
	float best_perplexity;
	LZMAPacket best_packet;
} GreedyCompressor;

void greedy_compressor_new(GreedyCompressor* greedy, LZMAState* lzma_state, float total_perplexity);
void greedy_compressor_packet_callback(void* user_data, LZMAPacket packet);
