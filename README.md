# MEGALANIA

## Sometimes, you just need a big lizard

Megalania is an LZMA compressor that uses beam search to find an optimal sequence of LZMA packets to maximize the compression ratio. Because Megalania uses such a hefty search algorithm it uses a lot of memory (BEAM_SIZE * FILE_SIZE * 16 bytes). However, it may be able to compress better than standard LZMA compressors.

This is still a work in progress.
