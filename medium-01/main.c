#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "recaman.h"

void usage(char *arg0) { fprintf(stderr, "Usage: %s <i|r> <nterms>\n", arg0); }

int get_term_width() {
  struct winsize w;
  // I love TIOCGWISZ, it really rolls off the tongue.
  ioctl(fileno(stdout), TIOCGWINSZ, &w);
  return w.ws_col;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    usage(argv[0]);
    return 1;
  }

  size_t nterms = atoi(argv[2]);
  int64_t sequence[nterms];

  if (strcmp(argv[1], "r") == 0) {
    printf("Using the recursive algorithm.\n");
    recaman_recursive(nterms, sequence);
  } else if (strcmp(argv[1], "i") == 0) {
    printf("Using the iterative algorithm.\n");
    recaman_iterative(nterms, sequence);
  } else {
    usage(argv[0]);
    return 1;
  }
  printf("\n");

  printf("The Recamán's sequence for %ld terms is:\n", nterms);
  for (size_t i = 0; i < nterms; i++) {
    printf("%ld", sequence[i]);
    if (i < nterms - 1) {
      printf(", ");
    }
  }
  printf("\n");
  printf("\n");

  // Draw something pretty for the flare.
  printf("The vertical graph is:\n");
  draw_recaman(sequence, nterms, get_term_width());

  return 0;
}
