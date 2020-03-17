#include "lzma_state.h"

static void lzma_state_init_probs(LZMAProbabilityModel* probs)
{
	//this is probably undefined behaviour but whatever...
	int num_probs = sizeof(LZMAProbabilityModel)/sizeof(Prob);
	Prob* prob_array = (Prob*)probs;
	for (int i = 0; i < num_probs; i++) {
		prob_array[i] = PROB_INIT_VAL;
	}
}

void lzma_state_init(LZMAState* state, const unsigned char* data, size_t data_size)
{
	state->data = data;
	state->data_size = data_size;

	state->state = 0;
	lzma_state_init_probs(&state->probs);

	state->position = 0;
	state->perplexity = 0.f;
}
