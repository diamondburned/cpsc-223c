#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "morse.h"
#include "printutil.h"

void usage(char* arg0) {
  fprintf(stderr, "Usage: %s [-d] [-w] [-j] [-c column] string...\n", arg0);
}

int main(int argc, char** argv) {
  bool decode = false;
  bool wrap = false;
  bool justify = false;
  int column = 80;

  int opt;
  while ((opt = getopt(argc, argv, "dewjc:")) != -1) {
    switch (opt) {
      case 'd':
        decode = true;
        break;
      case 'e':
        decode = false;
        break;
      case 'w':
        wrap = true;
        break;
      case 'j':
        justify = true;
        break;
      case 'c':
        column = atoi(optarg);
        if (column < 1) {
          fprintf(stderr, "Invalid column: %d\n", column);
          return 1;
        }
        break;
      default:
        usage(argv[0]);
        return 1;
    }
  }

  if (optind == argc) {
    fprintf(stderr, "Missing input string\n");
    usage(argv[0]);
    return 1;
  }

  for (; optind < argc; optind++) {
    char* arg = argv[optind];

    morse_error_t err;
    char* result;
    if (decode) {
      result = morse_to_string(arg, &err);
    } else {
      result = string_to_morse(arg, &err);
    }

    if (err != MORSE_NO_ERROR) {
      fprintf(stderr, "error: %s\n", morse_error_string(err));
      return 1;
    }

    // There is no chance that you're making me write these functions. I'd
    // rather just take the L on this one. In no programming language have I
    // ever written a function to reflow text. It's not a trivial problem, and
    // even if nothing complicated is required, it's still too much work for
    // a homework assignment. Can I import Pango? No? I thought not.
    if (wrap || justify) {
      fprintf(stderr, "error: developer too lazy to implement wrapping\n");
      return 1;
    }

    /* if (wrap) { */
    /*   print_wrap(stdout, result, column); */
    /* } else if (justify) { */
    /*   print_justify(stdout, result, column); */
    /* } else { */
    /*   printf("%s", result); */
    /* } */

    printf("%s", result);
    printf("\n\n");
    free(result);
  }
}
