#include "beam_packet_history.h"
#include "memory_mapper.h"
#include <stdlib.h>
#include <stdio.h>

struct BeamPacketHistory_struct {
	LZMAPacket* packets;
	size_t data_size;
	size_t beam_size;
	size_t depth;
};

#define PACKET_HISTORY_SIZE(beam_size, data_size) (sizeof(LZMAPacket) * beam_size * data_size)

size_t beam_packet_history_memory_usage(size_t data_size, size_t beam_size)
{
	return sizeof(BeamPacketHistory) + PACKET_HISTORY_SIZE(beam_size, data_size);
}

BeamPacketHistory* beam_packet_history_new(size_t data_size, size_t beam_size)
{
	BeamPacketHistory* history = malloc(sizeof(BeamPacketHistory));
	if (history == NULL) {
		return NULL;
	}
	history->data_size = data_size;
	history->beam_size = beam_size;
	history->depth = 0;
	if (map_anonymous(PACKET_HISTORY_SIZE(beam_size, data_size), (uint8_t**) &history->packets) != 0) {
		free(history);
		return NULL;
	}
	return history;
}

void beam_packet_history_free(BeamPacketHistory* history)
{
	unmap((uint8_t*) history->packets, PACKET_HISTORY_SIZE(history->beam_size, history->data_size));
	free(history);
}

size_t beam_packet_history_beam_count(const BeamPacketHistory* history)
{
	return history->depth;
}

bool beam_packet_history_insert(BeamPacketHistory* history, const BeamEntry* entries, size_t entries_count)
{
	//todo: should these be asserts?
	if (entries_count > history->beam_size) {
		fprintf(stderr, "Tried to insert more beam entries than we have room for! max: %ld, got: %ld\n", history->beam_size, entries_count);
		return false;
	}
	if (history->depth >= history->data_size) {
		fprintf(stderr, "Tried to insert more beams than we have room for! max: %ld\n", history->data_size);
		return false;
	}
	size_t pos = history->depth++;
	for (size_t i = 0; i < entries_count; i++) {
		history->packets[pos * history->beam_size + i] = entries[i].packet;
	}
	return true;
}

bool beam_packet_history_get_path(const BeamPacketHistory* history, LZMAPacket* packets, size_t depth, size_t index)
{
	//todo: should these be asserts?
	if (index > history->beam_size) {
		fprintf(stderr, "Tried to get path for beam outside range! max: %ld, got: %ld\n", history->beam_size, index);
		return false;
	}
	if (depth >= history->data_size) {
		fprintf(stderr, "Tried to get path for depth outside range! max: %ld\n", history->data_size);
		return false;
	}

	do {
		LZMAPacket packet = history->packets[depth * history->beam_size + index];
		packets[depth] = packet;
		index = UNPACK_PARENT(packet.meta);
	} while (depth-- > 0);
	return true;
}
