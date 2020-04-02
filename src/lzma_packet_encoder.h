#pragma once
#include "lzma_state.h"
#include "lzma_packet.h"
#include "encoder_interface.h"

//This function encodes a packet given an LZMAState. It writes this data to the encoder interface.

void lzma_encode_packet(LZMAState* lzma_state, EncoderInterface* enc, LZMAPacket packet);
