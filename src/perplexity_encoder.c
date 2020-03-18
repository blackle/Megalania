#include "perplexity_encoder.h"
#include <math.h>

static void perplexity_encoder_encode_bit(EncoderInterface* enc, bool bit, Prob prob)
{
	float* perplexity = (float*)enc->private_data;
	*perplexity += -log2(bit ? (1.f - prob/2048.f) : (prob/2048.f));
}

void perplexity_encoder_new(EncoderInterface* enc, float* perplexity)
{
	enc->encode_bit = perplexity_encoder_encode_bit;
	enc->private_data = (void*)perplexity;
}
