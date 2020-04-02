#pragma once
#include "lzma_state.h"

//this writes the LZMA header for the supplied LZMAState to the specified file descriptor (fd)

void lzma_encode_header(const LZMAState* lzma_state, int fd);
