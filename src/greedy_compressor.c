#include "greedy_compressor.h"
#include "lzma_packet_encoder.h"
#include "perplexity_encoder.h"
#include <string.h>

void greedy_compressor_new(GreedyCompressor* greedy, LZMAState* lzma_state, float total_perplexity)
{
	greedy->lzma_state = lzma_state;
	greedy->total_perplexity = total_perplexity;
	greedy->best_perplexity = -1;
}

void greedy_compressor_packet_callback(void* user_data, LZMAPacket packet)
{
	GreedyCompressor* greedy = (GreedyCompressor*)user_data;
	LZMAState new_lzma_state;
	memcpy(&new_lzma_state, greedy->lzma_state, sizeof(LZMAState));
	float perplexity = greedy->total_perplexity;
	EncoderInterface perplexity_enc;
	perplexity_encoder_new(&perplexity_enc, &perplexity);
	lzma_encode_packet(&new_lzma_state, &perplexity_enc, packet);

	size_t packet_len = greedy->lzma_state->position + UNPACK_LEN(packet.match);
	size_t best_packet_len = greedy->lzma_state->position + UNPACK_LEN(greedy->best_packet.match);
	if (greedy->best_perplexity < 0.f || perplexity*best_packet_len < greedy->best_perplexity*packet_len) {
		greedy->best_perplexity = perplexity;
		greedy->best_packet = packet;
	}
}
