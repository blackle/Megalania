#pragma once
#include "beam_entry.h"

typedef struct Beam_struct Beam;

Beam* beam_new(size_t beam_size);
void beam_free(Beam* beam);

size_t beam_count(const Beam* beam);
void beam_clear(Beam* beam);

void beam_insert(Beam* beam, const BeamEntry* entry);
const BeamEntry* beam_entries(const Beam* beam);
