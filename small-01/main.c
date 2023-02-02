#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "prime.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <number>\n", argv[0]);
    return 1;
  }

  int num;
  if (sscanf(argv[1], "%d", &num) != 1) {
    printf("Error: invalid number: %s\n", argv[1]);
    return 1;
  }

  const int PFACTORS_LEN = 256;
  int pfactors[PFACTORS_LEN];

  int npfactors = prime_factors(num, pfactors, PFACTORS_LEN);
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
