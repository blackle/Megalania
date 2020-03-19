#include "greedy_compressor.h"
#include "lzma_packet_encoder.h"
#include "perplexity_encoder.h"
#include <string.h>

void greedy_compressor_new(GreedyCompressor* greedy, LZMAState* lzma_state)
{
	greedy->lzma_state = lzma_state;
	greedy->best_perplexity = -1;
}

void greedy_compressor_evaluate_packet(GreedyCompressor* greedy, LZMAPacket packet)
{
	LZMAState new_lzma_state;
	memcpy(&new_lzma_state, greedy->lzma_state, sizeof(LZMAState));
	float perplexity = 0.f;
	EncoderInterface perplexity_enc;
	perplexity_encoder_new(&perplexity_enc, &perplexity);
	lzma_encode_packet(&new_lzma_state, &perplexity_enc, packet);
	perplexity /= UNPACK_LEN(packet.match);
	if (greedy->best_perplexity < 0.f || perplexity < greedy->best_perplexity) {
		greedy->best_perplexity = perplexity;
		greedy->best_packet = packet;
	}
}

void greedy_substring_callback(void* user_data, size_t offset, size_t length)
{
	GreedyCompressor* greedy = (GreedyCompressor*)user_data;
	unsigned dist = greedy->lzma_state->position - offset - 1;
	greedy_compressor_evaluate_packet(greedy, match_packet(0, dist, length));
	for (int i = 0; i < 4; i++) {
		if (dist == greedy->lzma_state->dists[i]) {
			greedy_compressor_evaluate_packet(greedy, long_rep_packet(0, i, length));
		}
	}
}
