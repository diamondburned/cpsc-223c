#include "wc.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// is_whitespace returns true if the given character is a whitespace character.
bool is_whitespace(char c) {
  switch (c) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
      return true;
    default:
      return false;
  }
}

// is_newline returns true if the given character is a newline character.
bool is_newline(char c) {
  switch (c) {
    case '\n':  // Linux
    case '\r':  // Windows
      return true;
    default:
      return false;
  };
}

void wc_count(const char* str, bool is_whitespace(char), wc_results* results) {
  const char* curr = str + 1;
  for (; *curr; curr++) {
    // We're counting words. Assume each word is separated by at least one
    // whitespace character. We ensure that the current character is a
    // whitespace but not the previous one.
    if (is_whitespace(*curr) && !is_whitespace(*(curr - 1))) {
      results->count++;
    }
  }

  // If the last word is not followed by a whitespace character, then we need to
  // increment the count.
  if (!is_whitespace(*(curr - 1))) {
    results->count++;
  }
}

void wc_str(const char* str, wc_mode mode, wc_flags flags,
            wc_results* results) {
  if (*str == '\0') {
    return;
  }

  switch (mode) {
    case WC_BYTES:
      results->count += strlen(str);
      break;
    case WC_LINES:
      wc_count(str, is_newline, results);
      break;
    case WC_WORDS:
      wc_count(str, is_whitespace, results);
      break;
  }

  if (flags.count_frequency) {
    for (const char* curr = str; *curr; curr++) {
      results->count_frequency[(unsigned char)*curr]++;
    }
  }

  if (flags.max_line_length) {
    int max = 0;
    int curr_len = 0;

    for (const char* curr = str; *curr; curr++) {
      if (is_newline(*curr)) {
        if (curr_len > max) {
          max = curr_len;
        }
        curr_len = 0;
      } else {
        curr_len++;
      }
    }

    if (curr_len > max) {
      max = curr_len;
    }

    results->max_line_length = max;
  }
}

// read_all consumes all of f. The output string is allocated with malloc and
// must be freed by the caller. The length of the string is returned. If an
// error occurs, 0 is returned, and out is set to NULL.
//
// For convenience, the buffer is allocated to be one byte longer than the
// length of the file, and the last byte is set to '\0'.
size_t read_all(FILE* f, char** out) {
  // Can we know the file size? If yes, then we can just allocate exactly that
  // much.
  if (fseek(f, 0, SEEK_END) == 0) {
    int fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    *out = malloc(fsize + 1);

    if (fread(*out, fsize, 1, f) != 1) {
      free(*out);
      *out = NULL;
      return 0;
    }

    (*out)[fsize] = '\0';
    return fsize;
  }

  const size_t START_SIZE = 4096;
  const size_t GROW = 2;

  size_t n = 0;
  size_t size = START_SIZE;
  *out = malloc(size);

  // We'll do this the Go way (or the any-sane-programming-language way). We'll
  // keep trying the buffer until we fill it, in which case we'll double the
  // size and try again.
  while (true) {
    n += fread(*out + n, 1, size - n - 1, f);
    if (ferror(f)) {
      free(*out);
      *out = NULL;
      return 0;
    }

    // Note to self: man 3 fread states that "if an error occurs, or the end of
    // the file is reached, the return value is a short item count (or zero)."
    // This implies that n < size is only true if we've reached the end of the
    // file.
    if (n < size - 1) {
      (*out)[n] = '\0';
      return n;
    }

    size *= GROW;
    *out = realloc(*out, size);
    fprintf(stderr, "realloc to %zu\n", size);
  }
}

bool wc(FILE* f, wc_mode mode, wc_flags flags, wc_results* results) {
  // Cheat because I'm not going to try and implement stream scanning in C.
  // That sounds like a massive pain.
  char* buf;
  size_t n = read_all(f, &buf);
  if (buf == NULL) {
    return false;
  }

  buf[n] = '\0';
  wc_str(buf, mode, flags, results);

  free(buf);
  return true;
}
