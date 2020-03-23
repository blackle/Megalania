#include "perplexity_encoder.h"
#include <math.h>

//https://stackoverflow.com/a/28730362
static float mFast_Log2(float val) {
	union { float val; int32_t x; } u = { val };
	register float log_2 = (float)(((u.x >> 23) & 255) - 128);
	u.x   &= ~(255 << 23);
	u.x   += 127 << 23;
	log_2 += ((-0.34484843f) * u.val + 2.02466578f) * u.val - 0.67487759f;
	return (log_2);
}

static void perplexity_encoder_encode_bit(EncoderInterface* enc, bool bit, Prob prob)
{
	//todo: use fixed point instead of float for speed reasons
	float* perplexity = (float*)enc->private_data;
	*perplexity += -mFast_Log2(bit ? (1.f - prob/2048.f) : (prob/2048.f));
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
