#include "perplexity_encoder.h"
#include <math.h>

static void perplexity_encoder_encode_bit(EncoderInterface* enc, bool bit, Prob prob)
{
	float* perplexity = (float*)enc->private_data;
	*perplexity += -log2(bit ? (1.f - prob/2048.f) : (prob/2048.f));
}

static void perplexity_encoder_encode_direct_bits(EncoderInterface* enc, unsigned bits, unsigned num_bits)
{
	(void) bits;
	float* perplexity = (float*)enc->private_data;
	*perplexity += num_bits;
}

void perplexity_encoder_new(EncoderInterface* enc, float* perplexity)
{
	enc->encode_bit = perplexity_encoder_encode_bit;
	enc->encode_direct_bits = perplexity_encoder_encode_direct_bits;
	enc->private_data = (void*)perplexity;
}
