#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ascii.h"

int main(int argc, char *argv[]) {
  if (argc != 1) {
    fprintf(stderr, "Usage: %s\n", argv[0]);
    return 1;
  }

  print_ascii_table(stdout);
  return 0;
}
