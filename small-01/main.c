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

  const int pfactors_len = 256;
  int pfactors[pfactors_len];

  int npfactors = prime_factors(num, pfactors, pfactors_len);
  if (npfactors == -1) {
    printf("Error: failed to find prime factors\n");
    return 1;
  }

  if (npfactors == 0) {
    printf("the number %d is prime.\n", num);
    return 0;
  }

  printf("the number %d has the prime factors ", num);
  for (int i = 0; i < npfactors; i++) {
    printf("%d", pfactors[i]);
    if (i != npfactors - 1) {
      printf(", ");
    }
  }
  printf(".\n");
  return 0;
}

// prime_factors finds the prime factors of the given number into the given
// factors array. The returned value is the number of factors found, or -1 if an
// error occured.
int prime_factors(int num, int *dst, int dst_len) {
  // Hack to reuse our prime_factors function by allocating a tiny factors
  // buffer.
  int pfactors_tmp[1];
  int ndst = 0;
  for (int i = 2; i <= num / 2; i++) {
    // Check if i is a factor of num.
    if (num % i == 0) {
      // If it is, then we need to check if i itself is prime, since we only
      // want to return prime factors.
      // The number is not prime.
      if (prime_factors(i, pfactors_tmp, 1) == 0) {
        // No factors were found, so i is prime.
        // Do a bound check before adding the factor.
        if (ndst == dst_len) {
          return -1;
        }
        dst[ndst] = i;
        ndst++;
      }
    }
  }
  return ndst;
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
