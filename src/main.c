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
#include "lzma_packet.h"
#include "lzma_packet_encoder.h"
#include "packet_enumerator.h"
#include "greedy_compressor.h"

#define GIGABYTE 1073741824

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
	uint32_t dictsize = 0x400000; //todo: peg this to file size
	uint64_t outsize = file_size;
	write(1, &props, 1);
	write(1, &dictsize, sizeof(uint32_t));
	write(1, &outsize, sizeof(uint64_t));

	EncoderInterface enc;
	range_encoder_new(&enc, 1);

	if (packet_enumerator_memory_usage(file_size) > GIGABYTE) {
		fprintf(stderr, "avoiding allocating more than a gigabyte of memory\n");
		return -1;
	}
	PacketEnumerator* packet_enumerator = packet_enumerator_new(file_data, file_size);

	float total_perplexity = 0.f;
	while (state.position < file_size) {
		GreedyCompressor greedy;
		greedy_compressor_new(&greedy, &state, total_perplexity);
		packet_enumerator_callback(packet_enumerator, &state, greedy_compressor_packet_callback, &greedy);
		total_perplexity = greedy.best_perplexity;
		//todo: make this a debug option?
		// unsigned packetlen = UNPACK_LEN(greedy.best_packet.match);
		// size_t oldpos = state.position;
		lzma_encode_packet(&state, &enc, greedy.best_packet);
		// fprintf(stderr, "packet: %d, %d, %d\t", UNPACK_TYPE(greedy.best_packet.meta), UNPACK_DIST(greedy.best_packet.match), packetlen);
		// for (size_t i = oldpos; i < oldpos+packetlen; i++) {
		// 	fprintf(stderr, "%c", file_data[i]);
		// }
		// fprintf(stderr, "\n");
	}

	range_encoder_free(&enc);

	packet_enumerator_free(packet_enumerator);

	if (unmap(file_data, file_size) < 0) {
		return -1;
	}

	return 0;
}