#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "probability.h"
#include "encoder_interface.h"

void encode_bit(bool bit, Prob* prob, EncoderInterface* enc);
void encode_direct_bits(unsigned bits, size_t num_bits, EncoderInterface* enc);

void encode_bit_tree(unsigned bits, Prob* probs, size_t num_bits, EncoderInterface* enc);
void encode_bit_tree_reverse(unsigned bits, Prob* probs, size_t num_bits, EncoderInterface* enc);

//todo: put in different file
#if 0
#define NUM_POS_BITS_MAX 4
#define LOW_CODER_BITS 3
#define MID_CODER_BITS 3
#define HIGH_CODER_BITS 8

typedef struct {
	Prob choice_1;
	Prob choice_2;

	Prob low_coder[1 << NUM_POS_BITS_MAX][1 << LOW_CODER_BITS];
	Prob mid_coder[1 << NUM_POS_BITS_MAX][1 << MID_CODER_BITS];
	Prob high_coder[1 << HIGH_CODER_BITS];
} LengthProbabilityModel;

#define LIT_PROBS_SIZE 0x300

typedef struct {
	Prob probs[LIT_PROBS_SIZE];
} LiteralProbabilityModel;

#define NUM_LEN_TO_POS_STATES 4
#define POS_SLOT_BITS 6
#define ALIGN_BITS 4
#define END_POS_MODEL_INDEX 14
#define NUM_FULL_DISTANCES (1 << (END_POS_MODEL_INDEX >> 1))

typedef struct {
	Prob pos_slot_coder[NUM_LEN_TO_POS_STATES][1 << POS_SLOT_BITS];
	Prob align_coder[1 << ALIGN_BITS];
	Prob pos_coder[1 + NUM_FULL_DISTANCES - END_POS_MODEL_INDEX];
} DistanceProbabilityModel;

#define NUM_STATES 12

typedef struct {
	Prob is_match[NUM_STATES << NUM_POS_BITS_MAX];
	Prob is_rep[NUM_STATES];
	Prob is_rep_g0[NUM_STATES];
	Prob is_rep_g1[NUM_STATES];
	Prob is_rep_g2[NUM_STATES];
	Prob is_rep0_long[NUM_STATES << NUM_POS_BITS_MAX];
} StateProbabilityModel;

typedef struct {
	LiteralProbabilityModel lit;
	LengthProbabilityModel len;
	LengthProbabilityModel rep_len;
	DistanceProbabilityModel dist;
	StateProbabilityModel state;
} ProbabilityModel;
#endif
