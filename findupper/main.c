#include <stdio.h>

char findFirstUpper(char *str) {
  if (*str == '\0') {
    return '\0';
  }
  if ('A' <= *str && *str <= 'Z') {
    return *str;
  }
  return findFirstUpper(&str[1]);
}

int main() {
  char *str = "hello World!";
  char upper = findFirstUpper(str);
  printf("%c\n", upper);
  return 0;
}
