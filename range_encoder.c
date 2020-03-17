#include "range_encoder.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define TOP_VALUE (1 << 24)

typedef struct {
	int fd;
	uint64_t low;
	uint32_t range;
	uint8_t cache;
	uint64_t cache_size;
} RangeEncoderData;

static void range_encoder_shift_low(EncoderInterface* enc)
{
	RangeEncoderData* data = (RangeEncoderData*)enc->private_data;
	if ((uint32_t)data->low < (uint32_t)0xFF000000 || (uint32_t)(data->low >> 32) != 0) {
		uint8_t temp = data->cache;
		do {
			uint8_t out_byte = (uint8_t)(temp + (uint8_t)(data->low >> 32));
			//todo: substitute fd with generic OutputInterface ?
			if (write(data->fd, &out_byte, 1) != 1) {
				fprintf(stderr, "could not write: %02x\n", out_byte);
			}
			temp = 0xFF;
		} while (--data->cache_size != 0);
		data->cache = (uint8_t)((uint32_t)data->low >> 24);
	}
	data->cache_size++;
	data->low = (uint32_t)data->low << 8;
}

static void range_encoder_flush_data(EncoderInterface* enc)
{
	for (int i = 0; i < 5; i++) {
		range_encoder_shift_low(enc);
	}
}


#define kTopMask ~((1 << 24) - 1)
	
#define kNumBitModelTotalBits 11
#define kBitModelTotal (1 << kNumBitModelTotalBits)
#define kNumMoveBits 5

static void range_encoder_encode_bit(EncoderInterface* enc, bool bit, Prob prob)
{
	RangeEncoderData* data = (RangeEncoderData*)enc->private_data;

	uint32_t new_bound = (data->range >> kNumBitModelTotalBits) * prob;
	if (bit) {
		data->low += (new_bound & 0xFFFFFFFFL);
		data->range -= new_bound;
	} else {
		data->range = new_bound;
	}
	// if (bit) {
	// 	uint32_t newBound = (data->range >> 14) * prob;
	// 	data->low += newBound;
	// 	data->range -= newBound;
	// } else {
	// 	data->range = (data->range >> 14) * prob;
	// }
	while ((data->range & kTopMask) == 0) {
		data->range <<= 8;
		range_encoder_shift_low(enc);
	}
}

	// public void Encode(short []probs, int index, int symbol) throws IOException
	// {
	// 	int prob = probs[index];
	// 	int newBound = (Range >>> kNumBitModelTotalBits) * prob;
	// 	if (symbol == 0)
	// 	{
	// 		Range = newBound;
	// 		probs[index] = (short)(prob + ((kBitModelTotal - prob) >>> kNumMoveBits));
	// 	}
	// 	else
	// 	{
	// 		Low += (newBound & 0xFFFFFFFFL);
	// 		Range -= newBound;
	// 		probs[index] = (short)(prob - ((prob) >>> kNumMoveBits));
	// 	}
	// 	if ((Range & kTopMask) == 0)
	// 	{
	// 		Range <<= 8;
	// 		ShiftLow();
	// 	}
	// }

void range_encoder_new(EncoderInterface* enc, int fd)
{
	RangeEncoderData* data = malloc(sizeof(RangeEncoderData));
	data->fd = fd;
	data->low = 0;
	data->range = 0xFFFFFFFF;
	data->cache = 0;
	data->cache_size = 1;

	enc->encode_bit = range_encoder_encode_bit;
	enc->private_data = (void*)data;
}

void range_encoder_free(EncoderInterface* enc)
{
	range_encoder_flush_data(enc);
	free(enc->private_data);
}
