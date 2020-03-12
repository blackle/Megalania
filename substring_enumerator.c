#include "substring_enumerator.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N_CHARS 256

struct SubstringEnumerator_struct {
	const unsigned char* data;
	size_t data_size;

	size_t bigram_offsets[N_CHARS][N_CHARS];
	size_t bigram_sizes[N_CHARS][N_CHARS];
	size_t* bigram_positions;
};

size_t substring_enumerator_memory_usage(size_t data_size)
{
	return sizeof(size_t) * data_size + sizeof(SubstringEnumerator);
}

void memoize_bigram_positions(SubstringEnumerator* em)
{
	const unsigned char* data = em->data;
	size_t data_size = em->data_size;

	for (size_t i = 1; i < data_size; i++) {
		em->bigram_sizes[data[i-1]][data[i]]++;
	}
	for (int i = 1; i < N_CHARS*N_CHARS; i++) {
		int j = i-1;
		em->bigram_offsets[i%N_CHARS][i/N_CHARS] = em->bigram_offsets[j%N_CHARS][j/N_CHARS] + em->bigram_sizes[j%N_CHARS][j/N_CHARS];
	}

	int fills[N_CHARS][N_CHARS] = {0};
	for (size_t i = 1; i < data_size; i++) {
		unsigned char first = data[i-1];
		unsigned char second = data[i];
		size_t offset = em->bigram_offsets[first][second] + fills[first][second];
		em->bigram_positions[offset] = i-1;
		fills[first][second]++;
	}
}

SubstringEnumerator* substring_enumerator_new(const unsigned char* data, size_t data_size)
{
	SubstringEnumerator* enumerator = malloc(sizeof(SubstringEnumerator));
	if (enumerator == NULL) {
		fprintf(stderr, "Error: could not allocate memory in substring_enumerator_new\n");
		return NULL;
	}
	memset(enumerator, 0, sizeof(SubstringEnumerator));
	enumerator->data = data;
	enumerator->data_size = data_size;

	//todo: put the mmap stuff in a helper function
	void* bigram_positions = mmap(0, sizeof(size_t) * data_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	if (bigram_positions == MAP_FAILED) {
		free(enumerator);
		return NULL;
	}
	enumerator->bigram_positions = bigram_positions;

	memoize_bigram_positions(enumerator);

	return enumerator;
}
void substring_enumerator_free(SubstringEnumerator* enumerator)
{
	//todo: ummap the bigram_positions array
	free(enumerator);
}

void substring_enumerator_callback(const SubstringEnumerator* em, size_t pos, size_t min_length, size_t max_length, SubstringEnumeratorCallback callback, void* user_data)
{
	//todo: put the min/max length in constructor so we can do the check there?
	if (min_length != 2) {
		fprintf(stderr, "Error: only min_length = 2 is supported in substring_enumerator_callback\n");
		return;
	}
	if (pos == 0) return;
	if (pos == em->data_size - 1) return;

	unsigned char first = em->data[pos];
	unsigned char second = em->data[pos+1];
	size_t offset = em->bigram_offsets[first][second];
	size_t size = em->bigram_sizes[first][second];
	for (size_t i = 0; i < size; i++) {
		size_t position = em->bigram_positions[offset + i];

		// if (position + dictionary_length < pos) continue; //this is supposed to be dictionary size
		if (position >= pos) break;
		callback(user_data, position, 2);
		for (size_t j = 2; j < max_length && j + pos < em->data_size; j++) {
			if (em->data[pos+j] != em->data[position+j]) break;
			callback(user_data, position, 1+j);
		}
	}
}
