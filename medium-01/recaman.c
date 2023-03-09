#include "recaman.h"

#include <stdlib.h>
#include <string.h>

#include "ints.h"

int64_t recaman_recursive_at(size_t n);

bool recaman_recursive_contains(size_t n, int64_t term) {
  if (n == 0) {
    return term == 0;
  }
  if (term == recaman_recursive_at(n)) {
    return true;
  }
  return recaman_recursive_contains(n - 1, term);
}

int64_t recaman_recursive_at(size_t n) {
  if (n == 0) {
    return 0;
  }

  static int64_t* cache = NULL;
  static size_t cache_size = 0;
  if (cache_size < n + 1) {
    // It's actually not too slow to realloc exactly n items here, because the
    // function always starts at the largest n and works its way down.
    cache = reallocarray(cache, n + 1, sizeof(int64_t));
    // Initialize memory for the new region.
    int64_t* start = &cache[cache_size];
    memset(start, 0, &cache[n] - start);
  }

  // Check if cached.
  if (cache[n] != 0) {
    return cache[n];
  }

  int64_t prev = recaman_recursive_at(n - 1);
  int64_t curr = prev - n;

  if (curr <= 0 || recaman_recursive_contains(n, curr)) {
    curr = prev + n;
  }

  cache[n] = curr;
  return curr;
}

void recaman_recursive(size_t max, int64_t out[]) {
  // Go backwards so we can use the cache.
  for (size_t n = max; n > 0; n--) {
    out[n] = recaman_recursive_at(n - 1);
  }
}

void recaman_iterative(size_t max, int64_t out[]) {
  out[0] = 0;
  for (size_t n = 1; n < max; n++) {
    int64_t prev = out[n - 1];
    int64_t curr = prev - n;
    if (curr <= 0 || has_int64(out, n, curr)) {
      curr = prev + n;
    }
    out[n] = curr;
  }
}

void draw_recaman(int64_t sequence[], size_t nterms, int term_width) {
  int64_t max = max_int64(sequence, nterms);
  int64_t prev_percent = -1;

  for (size_t n = 0; n < nterms; n++) {
    int64_t curr_percent = (sequence[n] * term_width) / max;

    // Go up until the previous point.
    for (int64_t i = 0; i < min64(curr_percent, prev_percent); i++) {
      printf(" ");
    }

    if (curr_percent > prev_percent) {
      // Fill the dot until our current point.
      for (int64_t i = prev_percent; i < curr_percent - 1; i++) {
        printf("-");
      }
      printf(">");
    } else {
      printf("<");
      // Fill the dot until our current point.
      for (int64_t i = curr_percent + 1; i < prev_percent; i++) {
        printf("-");
      }
    }

    printf("\n");

    prev_percent = curr_percent;
  }
}
