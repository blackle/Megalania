#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "memory_mapper.h"
#include "range_encoder.h"
#include "lzma_state.h"
#include "lzma_packet_encoder.h"
#include "perplexity_encoder.h"

#define GIGABYTE 1073741824

//todo: write tests!!!
//todo: add like, docstrings or something to explain what each structure does/what each function does

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

	if (file_size == 0) {
		return 0;
	}

	//todo: encapsulate in some kind of header serializer, write the data properly
	char props = 0;
	uint32_t dictsize = 0x400000; //todo: peg this to file size
	uint64_t outsize = file_size;
	write(1, &props, 1);
	write(1, &dictsize, sizeof(uint32_t));
	write(1, &outsize, sizeof(uint64_t));

	LZMAState state;
	lzma_state_init(&state, file_data, file_size);
	EncoderInterface enc;
	range_encoder_new(&enc, 1);
	for (size_t i = 0; i < file_size; i++) {
		lzma_encode_packet(&state, &enc, literal_packet());
	}
	range_encoder_free(&enc);

	if (unmap(file_data, file_size) < 0) {
		return -1;
	}

	return 0;
}