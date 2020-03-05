#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct {
	size_t sizes[256];
	size_t offsets[256];
	size_t* positions;
	const MmapReference* file;
} SubstringMap;

size_t substring_map_memory_size(const MmapReference* file) {
	return file->size * sizeof(size_t);
}

SubstringMap* substring_map_new(const MmapReference* file) {
	SubstringMap* map = malloc(sizeof(SubstringMap));
	memset(map, 0, sizeof(SubstringMap));
	map->file = file;

	void* positions = mmap(0, substring_map_memory_size(file), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	if (positions == MAP_FAILED) {
		free(map);
		return 0;
	}
	map->positions = positions;

	//places the positions of each character in sorted bins
	for (size_t i = 0; i < file->size; i++) {
		map->sizes[file->data[i]]++;
	}
	for (int i = 1; i < 256; i++) {
		map->offsets[i] = map->offsets[i-1] + map->sizes[i-1];
	}
	int fills[256] = {0};
	for (size_t i = 0; i < file->size; i++) {
		unsigned char byte = file->data[i];
		size_t offset = map->offsets[byte] + fills[byte];
		map->positions[offset] = i;
		fills[byte]++;
	}
	return map;
}

void substring_map_print_substrings(const SubstringMap* map, size_t dictionary_size, size_t curr_position, const unsigned char* query, size_t query_size) {
	if (query_size < 1) return;
	unsigned char byte = query[0];
	size_t offset = map->offsets[byte];
	size_t size = map->sizes[byte];
	int count = 0;
	for (size_t i = 0; i < size; i++) {
		size_t position = map->positions[offset + i];
		(void)dictionary_size;
		if (dictionary_size > 0 && position + dictionary_size < curr_position) continue;
		if (position >= curr_position) break;
		count++;
		// printf("%zd,%d ", position, 1);
		for (size_t j = 1; j < query_size && j + position < map->file->size; j++) {
			if (query[j] != map->file->data[position+j]) break;
			count++;
			// printf("%zd,%zd ", position, j+1);
		}
	}
	printf("%d\n", count);
}

void substring_map_free(SubstringMap* map) {
	munmap((void*) map->positions, substring_map_memory_size(map->file));
	free(map);
}

#define GIGABYTE 1073741824

int main(int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s filename\n", argv[1]);
		return -1;
	}

	MmapReference ref;
	if (map_file(argv[1], &ref) < 0) {
		return -1;
	}

	if (substring_map_memory_size(&ref) > GIGABYTE) {
		fprintf(stderr, "avoiding allocating more than a gigabyte of memory\n");
		return -1;
	}
	SubstringMap* map = substring_map_new(&ref);

	for (size_t i = 0; i < map->file->size; i++) {
		substring_map_print_substrings(map, 0x4000, i, map->file->data + i, 273);
	}

	substring_map_free(map);

	if (unmap_file(&ref) < 0) {
		return -1;
	}

	return 0;
}