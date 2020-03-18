#pragma once
#include <stddef.h>
#include <stdint.h>

int map_file(const char* filename, const uint8_t** data, size_t* data_size);
int map_anonymous(size_t data_size, uint8_t** data);

int unmap(const uint8_t* data, size_t data_size);
