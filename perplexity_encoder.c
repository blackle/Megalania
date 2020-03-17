#include "perplexity_encoder.h"
#include <math.h>

void perplexity_encoder(EncoderInterface* enc, bool bit, Prob prob)
{
	float* perplexity = (float*)enc->private_data;
	*perplexity += -log2(bit ? (1.f - prob/2048.f) : (prob/2048.f));
}

void perplexity_encoder_new(EncoderInterface* enc, float* perplexity)
{
	enc->encode_bit = perplexity_encoder;
	enc->private_data = (void*)perplexity;
}
