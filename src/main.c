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
#include "perplexity_encoder.h"
#include "beam.h"
#include "beam_entry.h"
#include "beam_packet_history.h"

#define GIGABYTE 1073741824

//todo: write tests!!!
//todo: add like, docstrings or something to explain what each structure does/what each function does

static void swap(Beam** a, Beam** b)
{
	Beam* tmp = *a;
	*a = *b;
	*b = tmp;
}

typedef struct {
	const BeamEntry* parent_entry;
	Beam* beam;
} BeamSearchPacketCallbackData;

static void beam_search_packet_callback(void* user_data, LZMAPacket packet)
{
	BeamSearchPacketCallbackData* data = (BeamSearchPacketCallbackData*) user_data;
	const BeamEntry* parent_entry = data->parent_entry;
	Beam* beam = data->beam;

	BeamEntry entry;
	memcpy(&entry, parent_entry, sizeof(BeamEntry));
	entry.packet = packet;

	EncoderInterface perplexity_encoder;
	perplexity_encoder_new(&perplexity_encoder, &entry.perplexity);
	lzma_encode_packet(&entry.lzma_state, &perplexity_encoder, entry.packet);
	entry.compression_ratio = entry.perplexity/entry.lzma_state.position;
	beam_insert(beam, &entry);
}

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

	if (packet_enumerator_memory_usage(file_size) > GIGABYTE) {
		fprintf(stderr, "avoiding allocating more than a gigabyte of memory\n");
		return -1;
	}
	PacketEnumerator* packet_enumerator = packet_enumerator_new(file_data, file_size);

	size_t beam_size = 25;
	Beam* current_beam = beam_new(beam_size);
	Beam* next_beam = beam_new(beam_size);
	BeamPacketHistory* history = beam_packet_history_new(file_size, beam_size);

	{
		//the first packet is always a literal (it can't be anything else)
		//todo: put in own function?
		BeamEntry first_literal;
		lzma_state_init(&first_literal.lzma_state, file_data, file_size);
		first_literal.packet = literal_packet(0);
		first_literal.perplexity = 0.f;

		EncoderInterface perplexity_encoder;
		perplexity_encoder_new(&perplexity_encoder, &first_literal.perplexity);
		lzma_encode_packet(&first_literal.lzma_state, &perplexity_encoder, first_literal.packet);
		first_literal.compression_ratio = first_literal.perplexity/first_literal.lzma_state.position;

		beam_insert(current_beam, &first_literal);
	}

	size_t depth = 0;
	float best_perplexity = -1.f;
	size_t best_depth = 0;
	size_t best_index = 0;

	while (beam_count(current_beam) > 0) {
		depth++;
		// if (depth > 11) return -1;
		// fprintf(stderr, "depth %ld/%ld - %ld\n", depth, file_size, beam_count(current_beam));
		size_t entries_count = beam_count(current_beam);
		const BeamEntry* entries = beam_entries(current_beam);
		if (!beam_packet_history_insert(history, entries, entries_count)) {
			fprintf(stderr, "failure at depth %ld\n", depth);
			return -1;
		}

		for (size_t parent = 0; parent < entries_count; parent++) {
			const BeamEntry* parent_entry = &entries[parent];
			// fprintf(stderr, "%f\t", parent_entry->compression_ratio);
			if (parent_entry->lzma_state.position == file_size) {
				if (best_perplexity < 0.f || parent_entry->perplexity < best_perplexity) {
					best_perplexity = parent_entry->perplexity;
					best_depth = depth-1;
					best_index = parent;
				}
				continue;
			}
			BeamSearchPacketCallbackData data = { .parent_entry = parent_entry, .beam = next_beam };
			packet_enumerator_callback(packet_enumerator, &parent_entry->lzma_state, parent, beam_search_packet_callback, &data);
		}
		// fprintf(stderr, "\n");

		swap(&next_beam, &current_beam);
		beam_clear(next_beam);
	}

	{
		LZMAPacket* packets = malloc(sizeof(LZMAPacket) * (best_depth+1));
		beam_packet_history_get_path(history, packets, best_depth, best_index);
		LZMAState state;
		lzma_state_init(&state, file_data, file_size);
		EncoderInterface enc;
		range_encoder_new(&enc, 1);
		for (size_t i = 0; i < best_depth+1; i++) {
			// unsigned packetlen = UNPACK_LEN(packets[i].match);
			// fprintf(stderr, "packet: %d, %d, %d\t", UNPACK_TYPE(packets[i].meta), UNPACK_DIST(packets[i].match), packetlen);
			// size_t oldpos = state.position;
			// for (size_t i = oldpos; i < oldpos+packetlen; i++) {
			// 	fprintf(stderr, "%c", file_data[i]);
			// }
			// fprintf(stderr, "\n");
			lzma_encode_packet(&state, &enc, packets[i]);
		}
		range_encoder_free(&enc);
	}

	beam_free(current_beam);
	beam_free(next_beam);


	packet_enumerator_free(packet_enumerator);

	if (unmap(file_data, file_size) < 0) {
		return -1;
	}

	return 0;
}