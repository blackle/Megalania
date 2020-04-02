#pragma once
#include "encoder_interface.h"

//This is a range encoder. It will write the range-encoded bits to a provided file descriptor

void range_encoder_new(EncoderInterface* enc, int fd);
void range_encoder_free(EncoderInterface* enc);
