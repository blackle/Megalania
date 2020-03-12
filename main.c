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

//todo: you can close fd and mmap is still valid
typedef struct {
	int fd;
	size_t size;
	const unsigned char* data;
} MmapReference;

int map_file(char* filename, MmapReference* ref) {
	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "could not open %s\n", filename);
		return -1;
	}
	struct stat buf;
	if (fstat(fd, &buf) < 0) {
		fprintf(stderr, "could not stat %s\n", filename);
		return -1;
	}
	void* map = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (map == MAP_FAILED) {
		fprintf(stderr, "could not mmap %s\n", filename);
		return -1;
	}
	ref->fd = fd;
	ref->size = buf.st_size;
	ref->data = (unsigned char*) map;
	return 0;
}

int unmap_file(MmapReference* ref) {
	if (munmap((void*) ref->data, ref->size) < 0) {
		fprintf(stderr, "could not munmap fd %d\n", ref->fd);
		return -1;
	}
	if (close(ref->fd) < 0) {
		fprintf(stderr, "could not close fd %d\n", ref->fd);
		return -1;
	}
	ref->fd = -1;
	ref->size = -1;
	ref->data = 0;
	return 0;
}

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

	MmapReference ref;
	if (map_file(argv[1], &ref) < 0) {
		return -1;
	}

	if (substring_enumerator_memory_usage(ref.size) > GIGABYTE) {
		fprintf(stderr, "avoiding allocating more than a gigabyte of memory\n");
		return -1;
	}
	SubstringEnumerator* enumerator = substring_enumerator_new(ref.data, ref.size);

	int count = 0;
	for (size_t i = 0; i < ref.size; i++) {
		printf("%ld\t",i);
		substring_enumerator_callback(enumerator, i, 2, MAX_SUBSTRING, substring_callback, &count);
		printf("\n");
	}
	printf("\n%d\n", count);

	substring_enumerator_free(enumerator);

	if (unmap_file(&ref) < 0) {
		return -1;
	}

	return 0;
}