#include "ascii.h"

#include <math.h>
#include <stdio.h>

bool is_printable(char c) { return c >= '!' && c <= '~'; }

void print_ascii_table(FILE* stream) {
  const int COLS = 8;
  const unsigned char MAX = 0x7F;  // ASCII standard

  for (int i = 0; i < MAX; i += COLS) {
    fprintf(stream, "| ");

    for (int col = 0; col < COLS; col++) {
      unsigned char c = i + col;
      fprintf(stream, "0x%02X ", c);

      if (is_printable(c)) {
        fprintf(stream, "%c", c);
      } else {
        fprintf(stream, "-");
      }

      fprintf(stream, " | ");
    }

    fprintf(stream, "\n");
  }
}
