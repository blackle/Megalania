#pragma once
#include <stdint.h>

#define INVALID 0
#define LITERAL 1
#define MATCH 2
#define SHORT_REP 3
#define LONG_REP_0 4
#define LONG_REP_1 5
#define LONG_REP_2 6
#define LONG_REP_3 7

#define MAX_PARENT_SIZE ((1 << 13) - 1)
#define PACK_META(parent, type) ((parent << 3) | (type & 0x7))
#define UNPACK_PARENT(meta) (meta >> 3)
#define UNPACK_TYPE(meta) (meta & 0x7)

#define PACK_MATCH(dist, len) ((dist << 9) | (len & 0x1FF))
#define UNPACK_DIST(match) (match >> 9)
#define UNPACK_LEN(match) (match & 0x1FF)

//this struct is optimized for size because we need to store BEAM_SIZE * FILE_SIZE many of them
typedef struct {
	uint16_t meta;
	union {
		unsigned char lit;
		uint16_t match;
	} data;
} LZMAPacket;