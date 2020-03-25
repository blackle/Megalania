#pragma once
#include "lzma_state.h"
#include "lzma_packet.h"

typedef struct {
	LZMAState lzma_state;
	LZMAPacket packet;
	float perplexity;
	float compression_ratio;
} BeamEntry;
