#pragma once
#include <stddef.h>

int map_file(const char* filename, const unsigned char** data, size_t* data_size);
int map_anonymous(size_t data_size, unsigned char** data);

int unmap(const unsigned char* data, size_t data_size);
