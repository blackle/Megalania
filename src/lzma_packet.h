#pragma once
#include <stdint.h>

#define INVALID 0
#define LITERAL 1
#define MATCH 2
#define SHORT_REP 3
#define LONG_REP 4

//this struct is optimized for size because we need to store BEAM_SIZE * FILE_SIZE many of them
typedef struct {
	uint8_t type;
	uint32_t dist;
	uint16_t len;
} LZMAPacket;

LZMAPacket literal_packet();
LZMAPacket match_packet(unsigned dist, unsigned len);
LZMAPacket short_rep_packet();
LZMAPacket long_rep_packet(unsigned dist_index, unsigned len);
