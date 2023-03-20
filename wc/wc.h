#pragma once

#include <stdbool.h>
#include <stdio.h>

typedef enum {
  WC_BYTES,  // -c
  WC_LINES,  // -l
  WC_WORDS,  // -w
} wc_mode;

typedef struct {
  bool max_line_length : 1;  // -L
  bool count_frequency : 1;  // -f
} wc_flags;

typedef struct {
  int count;                 // depends on mode
  int max_line_length;       // only if max_line_length is true
  int count_frequency[127];  // only if count_frequency is true
} wc_results;

// wc counts words in a file. If ferror(f) is true, wc returns false.
bool wc(FILE* f, wc_mode mode, wc_flags flags, wc_results* results);

// wc_str is like wc, but counts words in a string.
void wc_str(const char* str, wc_mode mode, wc_flags flags, wc_results* results);
