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
