#include "lzma_packet.h"

LZMAPacket literal_packet(int parent, uint8_t literal)
{
	LZMAPacket packet = { .meta = PACK_META(parent, LITERAL), .data = { .lit = literal } };
	return packet;
}

LZMAPacket match_packet(int parent, unsigned int dist, unsigned int len)
{
	LZMAPacket packet = { .meta = PACK_META(parent, MATCH), .data = { .match = PACK_MATCH(dist, len) } };
	return packet;
}

LZMAPacket short_rep_packet(int parent)
{
	LZMAPacket packet = { .meta = PACK_META(parent, SHORT_REP) };
	return packet;
}

// LZMAPacket long_rep_packet(int parent, int dist_index, int len)
// {

// }