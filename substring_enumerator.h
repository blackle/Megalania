#pragma once
#include <stddef.h>

typedef struct SubstringEnumerator_struct SubstringEnumerator;
typedef void (*SubstringEnumeratorCallback)(void* user_data, size_t offset, size_t length);

size_t substring_enumerator_memory_usage(size_t data_size);

SubstringEnumerator* substring_enumerator_new(const unsigned char* data, size_t data_size);
void substring_enumerator_free(SubstringEnumerator* enumerator);

void substring_enumerator_callback(const SubstringEnumerator* enumerator, size_t pos, size_t min_length, size_t max_length, SubstringEnumeratorCallback callback, void* user_data);
