#include "probability_model.h"

#define NUM_BIT_MODEL_TOTAL_BITS 11
#define NUM_MOVE_BITS 5

void encode_bit(bool bit, Prob* prob, EncoderInterface* enc)
{
	(*enc->encode_bit)(enc, bit, *prob);
	unsigned v = *prob;
	if (bit) {
    v -= v >> NUM_MOVE_BITS;
	} else {
		v += ((1 << NUM_BIT_MODEL_TOTAL_BITS) - v) >> NUM_MOVE_BITS;
	}
	*prob = (Prob)v;
}
