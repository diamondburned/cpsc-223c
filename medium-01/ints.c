#include "ints.h"

bool has_int64(int64_t *arr, size_t size, int64_t val) {
  for (size_t i = 0; i < size; i++) {
    if (arr[i] == val) {
      return true;
    }
  }
  return false;
}

int64_t max_int64(int64_t *arr, size_t size) {
  int64_t max = arr[0];
  for (size_t i = 1; i < size; i++) {
    if (arr[i] > max) {
      max = arr[i];
    }
  }
  return max;
}

int64_t min64(int64_t a, int64_t b) {
  if (a < b) {
    return a;
  }
  return b;
}
