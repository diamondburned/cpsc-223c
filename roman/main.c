#include <stdio.h>
#include <string.h>

int roman_numeral(char c) {
  switch (c) {
    case 'I':
      return 1;
    case 'V':
      return 5;
    case 'X':
      return 10;
    case 'L':
      return 50;
    case 'C':
      return 100;
    case 'D':
      return 500;
    case 'M':
      return 1000;
    default:
      return 0;
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <roman_numeral>\n", argv[0]);
    return 1;
  }

  const char* str = argv[1];
  const int len = strlen(str);
  int total = 0;

  for (int i = 0; i < len; i++) {
    int value = roman_numeral(str[i]);
    if (value == 0) {
      fprintf(stderr, "Invalid roman numeral: %c\n", str[i]);
    }

    if (i < len - 1) {
      int next = roman_numeral(str[i + 1]);
      if (next > value) {
        value *= -1;
      }
    }

    total += value;
  }

  printf("%d\n", total);
  return 0;
}
