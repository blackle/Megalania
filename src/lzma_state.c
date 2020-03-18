#include "lzma_state.h"
#include <string.h>

static void lzma_state_init_probs(LZMAProbabilityModel* probs)
{
	//this is probably undefined behaviour but whatever...
	int num_probs = sizeof(LZMAProbabilityModel)/sizeof(Prob);
	Prob* prob_array = (Prob*)probs;
	for (int i = 0; i < num_probs; i++) {
		prob_array[i] = PROB_INIT_VAL;
	}
}

void lzma_state_init(LZMAState* lzma_state, const unsigned char* data, size_t data_size)
{
	lzma_state->data = data;
	lzma_state->data_size = data_size;

	lzma_state->ctx_state = 0;
	memset(lzma_state->dists, 0, sizeof(lzma_state->dists));
	lzma_state_init_probs(&lzma_state->probs);

	lzma_state->position = 0;
}
