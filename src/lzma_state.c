#include <string.h>
#include "lzma_state.h"
#include "lzma_packet.h"

static void lzma_state_init_probs(LZMAProbabilityModel* probs)
{
	//this is probably undefined behaviour but whatever...
	int num_probs = sizeof(LZMAProbabilityModel)/sizeof(Prob);
	Prob* prob_array = (Prob*)probs;
	for (int i = 0; i < num_probs; i++) {
		prob_array[i] = PROB_INIT_VAL;
	}
}

void lzma_state_init(LZMAState* lzma_state, const uint8_t* data, size_t data_size)
{
	lzma_state->data = data;
	lzma_state->data_size = data_size;

	lzma_state->ctx_state = 0;
	memset(lzma_state->dists, 0, sizeof(lzma_state->dists));
	lzma_state_init_probs(&lzma_state->probs);

	lzma_state->position = 0;
}


void lzma_state_update_ctx_state(LZMAState* lzma_state, int packet_type)
{
	uint8_t ctx_state = lzma_state->ctx_state;
	switch (packet_type) {
		case LITERAL:
			if (ctx_state < 4) {
				ctx_state = 0;
			} else if (ctx_state < 10) {
				ctx_state -= 3;
			} else {
				ctx_state -= 6;
			}
			break;
		case MATCH:
			ctx_state = ctx_state < 7 ? 7 : 10;
			break;
		case SHORT_REP:
			ctx_state = ctx_state < 7 ? 9 : 11;
			break;
		case LONG_REP:
			ctx_state = ctx_state < 7 ? 8 : 11;
			break;
		case INVALID:
		default:
			//todo: assert or error messaging
			break;
	}
	lzma_state->ctx_state = ctx_state;
}
