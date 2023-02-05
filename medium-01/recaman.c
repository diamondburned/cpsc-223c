#include "recaman.h"

#include "ints.h"

int64_t recaman_recursive(size_t n, int64_t arr[]) {
  if (n == 0) {
    arr[0] = 0;
    return 0;
  }

  int64_t prev = recaman_recursive(n - 1, arr);
  int64_t curr = prev - n;
  if (curr <= 0 || has_int64(arr, n, curr)) {
    curr = prev + n;
  }

  arr[n] = curr;
  return curr;
}

void recaman_iterative(size_t max, int64_t arr[]) {
  arr[0] = 0;
  for (size_t n = 1; n < max; n++) {
    int64_t prev = arr[n - 1];
    int64_t curr = prev - n;
    if (curr <= 0 || has_int64(arr, n, curr)) {
      curr = prev + n;
    }
    arr[n] = curr;
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
