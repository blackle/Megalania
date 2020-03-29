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
#include "packet_enumerator.h"
#include "packet_slab.h"
#include "packet_slab_neighbour.h"
#include "top_k_packet_finder.h"

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

	PacketEnumerator* packet_enumerator = packet_enumerator_new(file_data, file_size);
	TopKPacketFinder* packet_finder = top_k_packet_finder_new(5, packet_enumerator);
	PacketSlab* packet_slab = packet_slab_new(file_size);
	LZMAPacket* packets = packet_slab_packets(packet_slab);

	{
		fprintf(stderr, "initializing\n");
		LZMAState lzma_state;
		lzma_state_init(&lzma_state, file_data, file_size);
		for (size_t i = 0; i < file_size; i++) {
			// for (size_t j = 0; j < (NUM_STATES << NUM_POS_BITS_MAX); j++) {
			// 	lzma_state.probs.ctx_state.is_match[j] = 1600;
			// }
			lzma_state.position = i;
			top_k_packet_finder_find(packet_finder, &lzma_state);
			top_k_packet_finder_pop(packet_finder, &packets[i]);
		}
		fprintf(stderr, "done!\n");
	}

	PacketSlabNeighbour neighbour;
	float best_perplexity = -1.f;
	srand(6666);
	int max_iters = 1000000;
	for (int i = 0; i < max_iters; i++) {
		// fprintf(stderr, "generating...\n");
		packet_slab_neighbour_new(&neighbour, packet_slab, file_data, file_size);
		packet_slab_neighbour_generate(&neighbour, packet_finder);

		if (best_perplexity < 0.f || neighbour.perplexity < best_perplexity) {
			best_perplexity = neighbour.perplexity;
			fprintf(stderr, "new best perplexity: %f\tat: %.1f%%\n", best_perplexity/8.f, ((float)i)/max_iters*100);
		} else {
			packet_slab_neighbour_undo(&neighbour);
		}

		packet_slab_neighbour_free(&neighbour);
	}

	top_k_packet_finder_free(packet_finder);
	packet_enumerator_free(packet_enumerator);

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
	while (state.position < state.data_size) {
		lzma_encode_packet(&state, &enc, packets[state.position]);
	}
	range_encoder_free(&enc);

	packet_slab_free(packet_slab);

	if (unmap(file_data, file_size) < 0) {
		return -1;
	}

	return 0;
}