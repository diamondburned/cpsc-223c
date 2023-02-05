#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// has_int64 is a function that returns true if the string contains an integer
// and false otherwise. Its range is [start, end).
bool has_int64(int64_t *arr, size_t size, int64_t val);

// max_int64 is a function that returns the maximum integer in the array.
int64_t max_int64(int64_t *arr, size_t size);

int64_t min64(int64_t a, int64_t b);
