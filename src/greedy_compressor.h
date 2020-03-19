#pragma once
#include "lzma_state.h"
#include "lzma_packet.h"

typedef struct {
	LZMAState* lzma_state;
	float best_perplexity;
	LZMAPacket best_packet;
} GreedyCompressor;

void greedy_compressor_new(GreedyCompressor* greedy, LZMAState* lzma_state);
void greedy_compressor_evaluate_packet(GreedyCompressor* greedy, LZMAPacket packet);
void greedy_substring_callback(void* user_data, size_t offset, size_t length);
