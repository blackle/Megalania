#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "substring_enumerator.h"
#include "memory_mapper.h"

#define MAX_SUBSTRING 273
#define GIGABYTE 1073741824

void substring_callback(void* user_data, size_t offset, size_t length)
{
	printf("%ld,%ld ", offset, length);
	int* count = (int*)user_data;
	*count += 1;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s filename\n", argv[1]);
		return -1;
	}

	const unsigned char* file_data;
	size_t file_size;
	if (map_file(argv[1], &file_data, &file_size) < 0) {
		return -1;
	}

	if (substring_enumerator_memory_usage(file_size) > GIGABYTE) {
		fprintf(stderr, "avoiding allocating more than a gigabyte of memory\n");
		return -1;
	}
	SubstringEnumerator* enumerator = substring_enumerator_new(file_data, file_size);

	int count = 0;
	for (size_t i = 0; i < file_size; i++) {
		printf("%ld\t",i);
		substring_enumerator_callback(enumerator, i, 2, MAX_SUBSTRING, substring_callback, &count);
		printf("\n");
	}
	printf("\n%d\n", count);

	substring_enumerator_free(enumerator);

	if (unmap(file_data, file_size) < 0) {
		return -1;
	}

	return 0;
}