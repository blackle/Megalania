#pragma once
#include "lzma_state.h"

void lzma_encode_header(const LZMAState* lzma_state, int fd);
