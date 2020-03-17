#pragma once
#include "encoder_interface.h"

void range_encoder_new(EncoderInterface* enc, int fd);
void range_encoder_free(EncoderInterface* enc);
