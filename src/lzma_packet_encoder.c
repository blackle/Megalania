#include "lzma_packet_encoder.h"
#include "probability_model.h"

typedef struct {
	bool match;
	bool rep;
	bool b3;
	bool b4;
	bool b5;
} LZMAPacketHeader;

static void lzma_encode_packet_header(LZMAState* lzma_state, EncoderInterface* enc, LZMAPacketHeader head)
{
	ContextStateProbabilityModel* ctx_probs = &lzma_state->probs.ctx_state;

	unsigned int ctx_pos_bits = 0; //position context bits currently unsupported
	unsigned int ctx_state = lzma_state->ctx_state;
	unsigned int ctx_pos_state = (ctx_state << NUM_POS_BITS_MAX) + ctx_pos_bits;

	encode_bit(head.match, &ctx_probs->is_match[ctx_pos_state], enc);
	if (!head.match) {
		return;
	}

	encode_bit(head.rep, &ctx_probs->is_rep[ctx_state], enc);
	if (!head.rep) {
		return;
	}

	encode_bit(head.b3, &ctx_probs->is_rep_g0[ctx_state], enc);
	if (head.b3) {
		encode_bit(head.b4, &ctx_probs->is_rep0_long[ctx_pos_state], enc);
	} else {
		encode_bit(head.b4, &ctx_probs->is_rep_g1[ctx_state], enc);
		if (head.b4) {
			encode_bit(head.b5, &ctx_probs->is_rep_g0[ctx_state], enc);
		}
	}
}

static void lzma_encode_literal(LZMAState* lzma_state, EncoderInterface* enc, uint8_t lit)
{
	LZMAPacketHeader head = { .match = 0 };
	lzma_encode_packet_header(lzma_state, enc, head);

	LZMAProbabilityModel* probs = &lzma_state->probs;
	unsigned int symbol = 1;
	unsigned int lit_ctx = 0; //todo: literal context bits currently unsupported
	Prob *lit_probs = &probs->lit[0x300 * lit_ctx];

	bool matched = lzma_state->ctx_state >= 7;
	//todo: make 'get_byte_at_distance' function somewhere
  uint8_t match_byte = lzma_state->data[lzma_state->position - lzma_state->dists[0] - 1];
	for (int i = 7; i >= 0; i--) {
		bool bit = (lit >> i) & 1;
		unsigned int context = symbol;

		if (matched) {
			int match_bit = (match_byte >> i) & 1;
			context += ((1 + match_bit) << 8);
			matched = (match_bit == bit);
		}

		encode_bit(bit, &lit_probs[context], enc);
		symbol = (symbol << 1) | bit;
	}
	lzma_state->position++;
}

static void lzma_encode_short_rep(LZMAState* lzma_state, EncoderInterface* enc)
{
	LZMAPacketHeader head = { .match = 1, .rep = 1, .b3 = 0, .b4 = 0 };
	lzma_encode_packet_header(lzma_state, enc, head);

	lzma_state->position++;
}

void lzma_encode_packet(LZMAState* lzma_state, EncoderInterface* enc, LZMAPacket packet)
{
	int type = UNPACK_TYPE(packet.meta);
	switch (type) {
		case LITERAL:
			lzma_encode_literal(lzma_state, enc, packet.data.lit);
			break;
		case SHORT_REP:
			lzma_encode_short_rep(lzma_state, enc);
			break;
		case INVALID:
		default:
			//todo: assert or error messaging
			break;
	}
	lzma_state_update_ctx_state(lzma_state, type);
};