#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// recaman_recursive is a recursive implementation of the Recaman sequence.
int64_t recaman_recursive(size_t n, int64_t arr[]);

// recaman_iterative is an iterative implementation of the Recaman sequence.
void recaman_iterative(size_t max, int64_t arr[]);

// draw_recaman is a function that draws the Recaman sequence. The sequence is
// scaled horizontally to the given term_width.
void draw_recaman(int64_t sequence[], size_t nterms, int term_width);
