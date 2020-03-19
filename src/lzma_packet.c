#include "lzma_packet.h"

LZMAPacket literal_packet(unsigned parent)
{
	LZMAPacket packet = { .meta = PACK_META(parent, LITERAL) };
	return packet;
}

LZMAPacket match_packet(unsigned parent, unsigned dist, unsigned len)
{
	LZMAPacket packet = { .meta = PACK_META(parent, MATCH), .match = PACK_MATCH(dist, len) };
	return packet;
}

LZMAPacket short_rep_packet(unsigned parent)
{
	LZMAPacket packet = { .meta = PACK_META(parent, SHORT_REP) };
	return packet;
}

LZMAPacket long_rep_packet(unsigned parent, unsigned dist_index, unsigned len)
{
	LZMAPacket packet = { .meta = PACK_META(parent, LONG_REP), .match = PACK_MATCH(dist_index, len) };
	return packet;
}