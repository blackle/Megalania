#pragma once
#include "beam_entry.h"
#include "lzma_packet.h"
#include <stdbool.h>

typedef struct BeamPacketHistory_struct BeamPacketHistory;

size_t beam_packet_history_memory_usage(size_t data_size, size_t beam_size);

BeamPacketHistory* beam_packet_history_new(size_t data_size, size_t beam_size);
void beam_packet_history_free(BeamPacketHistory* history);

size_t beam_packet_history_beam_count(const BeamPacketHistory* history);

bool beam_packet_history_insert(BeamPacketHistory* history, const BeamEntry* entries, size_t entries_count);
bool beam_packet_history_get_path(const BeamPacketHistory* history, LZMAPacket* packets, size_t depth, size_t index);
