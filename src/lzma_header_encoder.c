#include "lzma_header_encoder.h"
#include <unistd.h>
#include <endian.h>

static uint8_t lzma_encode_header_properties(const LZMAState* lzma_state)
{
	const LZMAProperties* p = &lzma_state->properties;
	return ((p->pb * 5 + p->lp) * 9 + p->lc);
}

//todo: abstract writing interface?
void lzma_encode_header(const LZMAState* lzma_state, int fd)
{
	char props = lzma_encode_header_properties(lzma_state);
	write(fd, &props, sizeof(uint8_t));

	uint32_t dictsize = htole32(0x400000); //todo: peg this to data size
	write(fd, &dictsize, sizeof(uint32_t));

	uint64_t outsize = htole32(lzma_state->data_size);
	write(fd, &outsize, sizeof(uint64_t));
}
