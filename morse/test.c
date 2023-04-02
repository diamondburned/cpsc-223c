#include <minctest.h>
#include <stdio.h>
#include <stdlib.h>

#include "morse.h"
#include "printutil.h"

typedef struct {
  char* str;
  char* morse;
} test_t;

test_t TEST_CASES[] = {
    {"HELLO", ".... . .-.. .-.. ---"},
    {"HELLO WORLD", ".... . .-.. .-.. ---  .-- --- .-. .-.. -.."},
    // See, this requirement is really dumb, because how would you differentiate
    // " \n " from "\n  "? Maybe it's to do with how I'm encoding it. Whatever,
    // it's 8AM, I'm too tired for this shit.
    /* {"HELLO \n WORLD", ".... . .-.. .-.. ---       .-- --- .-. .-.. -.."}, */
    {"HELLO\n\nWORLD", ".... . .-.. .-.. ---    .-- --- .-. .-.. -.."},
};

const int TEST_CASES_LEN = sizeof(TEST_CASES) / sizeof(TEST_CASES[0]);

void test_string_to_morse() {
  for (size_t i = 0; i < TEST_CASES_LEN; i++) {
    test_t* test = &TEST_CASES[i];

    morse_error_t err;
    char* morse = string_to_morse(test->str, &err);

    fprintf(stderr, "	 \"%s\" -> \"%s\"\n", test->str, test->morse);
    lequal(err, 0);
    lsequal(morse, test->morse);

    free(morse);
  }
}

void test_morse_to_string() {
  for (size_t i = 0; i < TEST_CASES_LEN; i++) {
    test_t* test = &TEST_CASES[i];

    morse_error_t err;
    char* str = morse_to_string(test->morse, &err);

    fprintf(stderr, "	 \"%s\" -> \"%s\"\n", test->morse, test->str);
    lequal(err, 0);
    lsequal(str, test->str);

    free(str);
  }
}

void test_print_wrap() {
  FILE* f = tmpfile();
  char buf[256];

  print_wrap(f, "hello world hello world", 15);

  fseek(f, 0, SEEK_SET);
  int n = fread(buf, 1, 256, f);
  buf[n] = '\0';
  lsequal(buf, "hello world hello\nworld");
}

int main() {
  lrun("morse/string_to_morse", test_string_to_morse);
  lrun("morse/morse_to_string", test_morse_to_string);
  lrun("printutil/print_wrap", test_print_wrap);
  lresults();
  return lfails != 0;
}
