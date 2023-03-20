#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wc.h"

bool is_printable(char c) { return c >= ' ' && c <= '~'; }

char* plural(int qty, char* singular, char* plural) {
  return qty == 1 ? singular : plural;
}

void print_wc_results(char* name, wc_results* res, wc_mode mode,
                      wc_flags flags) {
  char* what;
  switch (mode) {
    case WC_WORDS:
      what = plural(res->count, "word", "words");
      break;
    case WC_LINES:
      what = plural(res->count, "line", "lines");
      break;
    case WC_BYTES:
      what = plural(res->count, "byte", "bytes");
      break;
  }

  printf("%s: %d %s\n", name, res->count, what);

  if (flags.count_frequency) {
    printf("  Character Frequency:\n");
    for (int i = 0; i < 127; i++) {
      if (res->count_frequency[i] > 0) {
        printf("    %c (0x%02X): %d\n", is_printable(i) ? (char)i : '-', i,
               res->count_frequency[i]);
      }
    }
  }

  if (flags.max_line_length) {
    printf("  Max line length: %d\n", res->max_line_length);
  }
}

void print_usage(char* arg0) {
  fprintf(stderr,
          "Usage: %s [-c|-l|-w] [-f] [-L] [--] [file...]\n"
          "\n"
          "Flags:\n"
          "  -c, --bytes:           print the byte counts\n"
          "  -l, --lines:           print the newline counts\n"
          "  -w, --words:           print the word counts\n"
          "  -L, --max-line-length: print the length of the longest line\n"
          "  -f:                    print the frequency of each character\n",
          arg0);
}

int main(int argc, char* argv[]) {
  // Flags
  int argc_start = argc;
  wc_mode mode = WC_WORDS;
  wc_flags flags = {};

  // Parse flags.
  for (int i = 1; i < argc; i++) {
    // If the argument is either a file or a literal "-", then stop parsing.
    if (argv[i][0] != '-' || strcmp(argv[i], "-") == 0) {
      argc_start = i;
      break;
    }

    // If the argument is "--", then we stop parsing, but the filenames start at
    // the next argument.
    if (strcmp(argv[i], "--") == 0) {
      argc_start = i + 1;
      break;
    }

    if (argv[i][1] == '-') {
      // Long flag parsing.
      if (strcmp(argv[i], "--help") == 0) {
        print_usage(argv[0]);
        return 0;
      } else if (strcmp(argv[i], "--bytes") == 0) {
        mode = WC_BYTES;
      } else if (strcmp(argv[i], "--lines") == 0) {
        mode = WC_LINES;
      } else if (strcmp(argv[i], "--words") == 0) {
        mode = WC_WORDS;
      } else if (strcmp(argv[i], "--max-line-length") == 0) {
        flags.max_line_length = true;
      } else {
        fprintf(stderr, "Unknown flag: %s\n", argv[i]);
        return 1;
      }
      continue;
    }

    for (int j = 1; argv[i][j]; j++) {
      switch (argv[i][j]) {
        case 'h':
          print_usage(argv[0]);
          return 0;
        case 'c':
          mode = WC_BYTES;
          break;
        case 'l':
          mode = WC_LINES;
          break;
        case 'w':
          mode = WC_WORDS;
          break;
        case 'f':
          flags.count_frequency = true;
          break;
        case 'L':
          flags.max_line_length = true;
          break;
        default:
          fprintf(stderr, "Unknown flag: -%c\n", argv[i][j]);
          return 1;
      }
    }
  }

  char** files = argv + argc_start;
  int nfiles = argc - argc_start;
  if (nfiles == 0) {
    // No files specified, so read from stdin.
    files = (char*[]){"-"};
    nfiles = 1;
  }

  for (int i = 0; i < nfiles; i++) {
    FILE* f;
    if (strcmp(files[i], "-") == 0) {
      f = stdin;
    } else {
      f = fopen(files[i], "r");
      if (f == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", files[i]);
        return 1;
      }
    }

    wc_results results = {};
    wc(f, mode, flags, &results);
    print_wc_results(files[i], &results, mode, flags);

    if (i != nfiles - 1) {
      printf("\n");
    }

    fclose(f);
  }

  return 0;
}
