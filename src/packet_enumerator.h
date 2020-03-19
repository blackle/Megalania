#pragma once
#include "lzma_state.h"
#include "lzma_packet.h"

typedef struct PacketEnumerator_struct PacketEnumerator;
typedef void (*PacketEnumeratorCallback)(void* user_data, LZMAPacket packet);

size_t packet_enumerator_memory_usage(size_t data_size);

PacketEnumerator* packet_enumerator_new(const uint8_t* data, size_t data_size);
void packet_enumerator_free(PacketEnumerator* enumerator);

void packet_enumerator_callback(const PacketEnumerator* enumerator, const LZMAState* state, PacketEnumeratorCallback callback, void* user_data);
