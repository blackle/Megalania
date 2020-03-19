#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "substring_enumerator.h"
#include "memory_mapper.h"
#include "probability_model.h"
#include "encoder_interface.h"
#include "perplexity_encoder.h"
#include "range_encoder.h"
#include "lzma_state.h"
#include "lzma_packet.h"
#include "lzma_packet_encoder.h"

#define MIN_SUBSTRING 2
#define MAX_SUBSTRING 273
#define GIGABYTE 1073741824

void substring_callback(void* user_data, size_t offset, size_t length)
{
	printf("%ld,%ld ", offset, length);
	int* count = (int*)user_data;
	*count += 1;
}

//todo: compress file using only literal packets to start, i.e. write the range coder
#define LIT_PROBS_SIZE 0x300

//todo: write tests!!!

int main(int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s filename\n", argv[0]);
		return -1;
	}

	const uint8_t* file_data;
	size_t file_size;
	if (map_file(argv[1], &file_data, &file_size) < 0) {
		return -1;
	}

	LZMAState state;
	lzma_state_init(&state, file_data, file_size);

	char props = 0;
	uint32_t dictsize = 0x4;
	uint64_t outsize = file_size;
	write(1, &props, 1);
	write(1, &dictsize, sizeof(uint32_t));
	write(1, &outsize, sizeof(uint64_t));

	EncoderInterface enc;
	range_encoder_new(&enc, 1);

	for (size_t i = 0; i < file_size; i++) {
		lzma_encode_packet(&state, &enc, literal_packet(0));
	}

	range_encoder_free(&enc);

/*
	if (substring_enumerator_memory_usage(file_size) > GIGABYTE) {
		fprintf(stderr, "avoiding allocating more than a gigabyte of memory\n");
		return -1;
	}
	SubstringEnumerator* enumerator = substring_enumerator_new(file_data, file_size, MIN_SUBSTRING, MAX_SUBSTRING);

	int count = 0;
	for (size_t i = 0; i < file_size; i++) {
		printf("%ld\t",i);
		substring_enumerator_callback(enumerator, i, substring_callback, &count);
		printf("\n");
	}
	printf("\n%d\n", count);

	substring_enumerator_free(enumerator);
*/

	if (unmap(file_data, file_size) < 0) {
		return -1;
	}

	return 0;
}