#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    int64_t start;
    int64_t stop;
} bottles_opts;

// bottles_opts_new creates a new bottles_opts with default values.
bottles_opts bottles_opts_new();

// bottles_opts_validate validates the given bottles_opts. An error string is
// returned if the opts are invalid, otherwise NULL is returned.
char* bottles_opts_validate(bottles_opts opts);

// bottles prints the bottles of beer song to the given file.
void bottles(bottles_opts bottles, FILE* out);
