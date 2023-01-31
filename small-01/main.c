#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool eatoi(const char *str, int *out);
int prime_factors(int num, int *factors, int factors_len);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <number>\n", argv[0]);
    return 1;
  }

  int num;
  if (!eatoi(argv[1], &num)) {
    printf("Error: invalid number: %s\n", argv[1]);
    return 1;
  }

  const int factors_len = 256;
  int factors[factors_len];

  int nfactors = prime_factors(num, factors, factors_len);
  if (nfactors == -1) {
    printf("Error: failed to find prime factors\n");
    return 1;
  }

  if (nfactors == 0) {
    printf("the number %d is prime.\n", num);
    return 0;
  }

  printf("the number %d has the factors ", num);
  for (int i = 0; i < nfactors; i++) {
    printf("%d", factors[i]);
    if (i != nfactors - 1) {
      printf(", ");
    }
  }
  printf(".\n");
  return 0;
}

// prime_factors finds the prime factors of the given number into the given
// factors array. The returned value is the number of factors found, or -1 if an
// error occured.
int prime_factors(int num, int *factors, int factors_len) {
  int nfactors = 0;
  for (int i = 2; i <= num / 2; i++) {
    if (num % i == 0) {
      factors[nfactors] = i;
      nfactors++;
      if (nfactors == factors_len) {
        return -1;
      }
    }
  }
  return nfactors;
}

// eatoi converts str to an integer. If the given str is invalid, then false
// is returned, errno is set and out is undefined, otherwise true is returned
// and out is set.
bool eatoi(const char *str, int *out) {
  // I didn't think much when my friend ranted about errno and how it's god
  // awful and how using it with strtol was horrible. Like, how bad could it
  // be?
  //
  // My God, it is awful. There's no good way to check for errors when you use
  // strtol!!! You have to set errno to 0 before calling it, and then check
  // if it's non-zero after calling it. That's crazy! Like, huh?? There's no
  // good way to handle errors for atoi OR strtol??? I'M LOSING IT SCOOBS.
  errno = 0;
  long n = strtol(str, NULL, 10);
  if (errno != 0) {
    return false;
  }

  // Overflow check.
  if (n > INT_MAX || n < INT_MIN) {
    errno = ERANGE;
    return false;
  }

  *out = (int)n;
  return true;
}
