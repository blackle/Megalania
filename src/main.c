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
#include "file_output.h"
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
	TopKPacketFinder* packet_finder = top_k_packet_finder_new(10, packet_enumerator);
	PacketSlab* packet_slab = packet_slab_new(file_size);
	PacketSlab* packet_slab_best = packet_slab_new(file_size);
	LZMAPacket* packets = packet_slab_packets(packet_slab);
	LZMAPacket* bestest_packets = packet_slab_packets(packet_slab_best);

	// for (size_t i = 0; i < file_size; i++) {
	// 	LZMAState demostate;
	// 	lzma_state_init(&demostate, file_data, file_size, properties);
	// 	demostate.position = i;
	// 	top_k_packet_finder_find(packet_finder, &demostate, bestest_packets);
	// 	while (top_k_packet_finder_pop(packet_finder, &bestest_packets[i]));
	// 	if (i % 100 == 0) {
	// 		fprintf(stderr, "precompute progress: %f\n", (float)i/file_size);
	// 	}
	// }

	uint64_t current_perplexity = 0;
	uint64_t best_perplexity = 0;

	const int num_epochs = 10;
	const int num_iters = 50000;
	for (unsigned epoch = 0; epoch < num_epochs; epoch++) {
	PacketSlabNeighbour neighbour;
	memcpy(packets, bestest_packets, sizeof(LZMAPacket) * file_size);
	current_perplexity = 0;
	srand(147351);
	for (int i = 0; i < num_iters; i++) {
		packet_slab_neighbour_new(&neighbour, packet_slab, init_state);
		bool success = packet_slab_neighbour_generate(&neighbour, packet_finder);
		if (!success) {
			i--;
			continue;
		}

		bool transition = rand() % (i*i+1) < sqrt(num_iters) / (epoch*epoch+1);
		if (current_perplexity == 0 || neighbour.perplexity < current_perplexity || transition) {
			current_perplexity = neighbour.perplexity;
			if (best_perplexity == 0 || current_perplexity < best_perplexity) {
				best_perplexity = current_perplexity;
				memcpy(bestest_packets, packets, sizeof(LZMAPacket) * file_size);
			}
		} else {
			//todo: double check that this actually undoes the neighbour generation
			packet_slab_neighbour_undo(&neighbour);
		}
		if (i % 100 == 0) {
			fprintf(stderr, "current file size: %f\tepoch: %04d - %.1f%%\n", 18+current_perplexity/16384.f, epoch, ((float)i)/num_iters*100);
		}

		packet_slab_neighbour_free(&neighbour);
	}
}

	top_k_packet_finder_free(packet_finder);
	packet_enumerator_free(packet_enumerator);

	OutputInterface output;
	file_output_new(&output, stdout);
	LZMAState state = init_state;
	lzma_encode_header(&state, &output);
	EncoderInterface enc;
	range_encoder_new(&enc, &output);
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