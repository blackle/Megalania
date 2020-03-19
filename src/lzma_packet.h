#pragma once
#include <stdint.h>

#define INVALID 0
#define LITERAL 1
#define MATCH 2
#define SHORT_REP 3
#define LONG_REP 4

//todo: mixed radix packing
#define NUM_TYPES 5
#define PACK_META(parent, type) ((parent * NUM_TYPES) + (type % NUM_TYPES))
#define UNPACK_PARENT(meta) (meta / NUM_TYPES)
#define UNPACK_TYPE(meta) (meta % NUM_TYPES)

#define NUM_LENS 238
#define PACK_MATCH(dist, len) ((dist * NUM_LENS) + (len % NUM_LENS))
#define UNPACK_DIST(match) (match / NUM_LENS)
#define UNPACK_LEN(match) (match % NUM_LENS)

//this struct is optimized for size because we need to store BEAM_SIZE * FILE_SIZE many of them
typedef struct {
	uint16_t meta;
	uint32_t match; //todo: is 32 bits enough?
} LZMAPacket;

LZMAPacket literal_packet(unsigned parent);
LZMAPacket match_packet(unsigned parent, unsigned dist, unsigned len);
LZMAPacket short_rep_packet(unsigned parent);
LZMAPacket long_rep_packet(unsigned parent, unsigned dist_index, unsigned len);
