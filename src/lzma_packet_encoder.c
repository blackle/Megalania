#include "lzma_packet_encoder.h"
#include "probability_model.h"

static void lzma_encode_literal(LZMAState* lzma_state, EncoderInterface* enc, uint8_t lit)
{
	LZMAProbabilityModel* probs = &lzma_state->probs;

	unsigned int posState = 0; //position context bits currently unsupported
	unsigned int is_match_ctx = (lzma_state->ctx_state << NUM_POS_BITS_MAX) + posState;
	encode_bit(0, &probs->ctx_state.is_match[is_match_ctx], enc);

	unsigned int symbol = 1;
	unsigned int lit_ctx = 0; //literal context bits currently unsupported
	Prob *lit_probs = &probs->lit[0x300 * lit_ctx];
	if (lzma_state->ctx_state >= 7) {
		//todo (note that implementing this will break the next thing)
	}
	for (int i = 0; i < 8; i++) {
		bool bit = lit & (1 << (7-i));
		encode_bit(bit, &lit_probs[symbol], enc);
		symbol = (symbol << 1) | bit;
	}
	//todo: update ctx_state here
}

    // unsigned prevByte = 0;
    // if (!OutWindow.IsEmpty())
    //   prevByte = OutWindow.GetByte(1);
    
    // unsigned symbol = 1;
    // unsigned litState = ((OutWindow.TotalPos & ((1 << lp) - 1)) << lc) + (prevByte >> (8 - lc));
    // CProb *probs = &LitProbs[(UInt32)0x300 * litState];
    
    // if (state >= 7)
    // {
    //   unsigned matchByte = OutWindow.GetByte(rep0 + 1);
    //   do
    //   {
    //     unsigned matchBit = (matchByte >> 7) & 1;
    //     matchByte <<= 1;
    //     unsigned bit = RangeDec.DecodeBit(&probs[((1 + matchBit) << 8) + symbol]);
    //     symbol = (symbol << 1) | bit;
    //     if (matchBit != bit)
    //       break;
    //   }
    //   while (symbol < 0x100);
    // }
    // while (symbol < 0x100)
    //   symbol = (symbol << 1) | RangeDec.DecodeBit(&probs[symbol]);
    // OutWindow.PutByte((Byte)(symbol - 0x100));

void lzma_encode_packet(LZMAState* lzma_state, EncoderInterface* enc, LZMAPacket packet)
{
	int type = UNPACK_TYPE(packet.meta);
	switch (type) {
		case LITERAL:
			lzma_encode_literal(lzma_state, enc, packet.data.lit);
			break;
		case INVALID:
		default:
			//todo: assert or error messaging
			break;
	};
};