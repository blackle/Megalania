#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "memory_mapper.h"
#include "range_encoder.h"
#include "lzma_state.h"
#include "lzma_header_encoder.h"
#include "lzma_packet_encoder.h"
#include "packet_enumerator.h"
#include "packet_slab.h"
#include "packet_slab_neighbour.h"
#include "top_k_packet_finder.h"

#define GIGABYTE 1073741824

//todo: write tests!!!
//todo: add like, docstrings or something to explain what each structure does/what each function does
//todo: inform user how much memory is expected to be used

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

	LZMAState init_state;
	LZMAProperties properties = { .lc = 0, .lp = 0, .pb = 0 };
	lzma_state_init(&init_state, file_data, file_size, properties);

	PacketEnumerator* packet_enumerator = packet_enumerator_new(file_data, file_size);
	TopKPacketFinder* packet_finder = top_k_packet_finder_new(20, packet_enumerator);
	PacketSlab* packet_slab = packet_slab_new(file_size);
	PacketSlab* packet_slab_best = packet_slab_new(file_size);
	LZMAPacket* packets = packet_slab_packets(packet_slab);
	LZMAPacket* bestest_packets = packet_slab_packets(packet_slab_best);

	srand(18931);

	uint64_t current_perplexity = 0;
	uint64_t best_perplexity = 0;
	PacketSlabNeighbour neighbour;
	const int max_iters = 60000;
	for (int i = 0; i < max_iters; i++) {
		packet_slab_neighbour_new(&neighbour, packet_slab, init_state);
		bool success = packet_slab_neighbour_generate(&neighbour, packet_finder);
		if (!success) {
			continue;
		}

		float diff = ((int64_t)current_perplexity - (int64_t)neighbour.perplexity)/(float)neighbour.perplexity;
		float time = ((float)i)/max_iters;
		float temp = pow(1.f - time, 1.f) * 0.0001f;
		float chance = (diff > 0) ? 1.0 : exp(diff / temp);
		bool transition = rand() % INT_MAX < chance * INT_MAX;
		if (current_perplexity == 0 || transition) {
			current_perplexity = neighbour.perplexity;
			if (best_perplexity == 0 || current_perplexity < best_perplexity) {
				best_perplexity = current_perplexity;
				memcpy(bestest_packets, packets, sizeof(LZMAPacket) * file_size);
			}
		} else {
			//todo: double check that this actually undoes the neighbour generation
			packet_slab_neighbour_undo(&neighbour);
		}
		if (i % 50 == 0) {
			fprintf(stderr, "current file size: %f\tat: %.1f%%\n", 18+current_perplexity/16384.f, ((float)i)/max_iters*100);
		}

		packet_slab_neighbour_free(&neighbour);
	}

	top_k_packet_finder_free(packet_finder);
	packet_enumerator_free(packet_enumerator);

	//todo: encapsulate in some kind of header serializer, write the data properly
	// char props = 0;
	// uint32_t dictsize = 0x400000; //todo: peg this to file size
	// uint64_t outsize = file_size;
	// write(1, &props, 1);
	// write(1, &dictsize, sizeof(uint32_t));
	// write(1, &outsize, sizeof(uint64_t));

	LZMAState state = init_state;
	lzma_encode_header(&state, 1);
	EncoderInterface enc;
	range_encoder_new(&enc, 1);
	while (state.position < state.data_size) {
		lzma_encode_packet(&state, &enc, bestest_packets[state.position]);
	}
	range_encoder_free(&enc);

	packet_slab_free(packet_slab_best);
	packet_slab_free(packet_slab);

	if (unmap(file_data, file_size) < 0) {
		return -1;
	}

	return 0;
}