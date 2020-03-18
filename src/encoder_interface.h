#pragma once
#include "probability.h"
#include <stdbool.h>

typedef struct EncoderInterface_struct EncoderInterface;

struct EncoderInterface_struct {
	void (*encode_bit)(EncoderInterface* enc, bool bit, Prob prob);
	void* private_data;
};
