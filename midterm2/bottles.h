#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

struct bottles_opts_ {
 int64_t start;
 int64_t stop;
};

typedef struct bottles_opts_ bottles_opts;

bottles_opts bottles_opts_new();

char* bottles_opts_validate(bottles_opts opts);

void bottles(bottles_opts bottles, FILE* out);
