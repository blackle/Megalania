#pragma once
#include "lzma_state.h"
#include "lzma_packet.h"
#include "encoder_interface.h"

void lzma_encode_packet(LZMAState* state, EncoderInterface* enc, LZMAPacket packet);
