#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "munit/munit.h"

int execute_unit_tests(int argc, char* argv[]);

void usage(char* argv[]) {
  fprintf(stderr,
          "Usage: %s from-base <base> <baseN> | to-base <base> <base10>\n",
          argv[0]);
}

int base10FromBase(int base, char* baseN) {
  int base10 = 0;
  for (int i = 0; baseN[i]; i++) {
    int digit;
    if (baseN[i] >= '0' && baseN[i] <= '9') {
      digit = baseN[i] - '0';
    } else if (baseN[i] >= 'A' && baseN[i] <= 'Z') {
      digit = baseN[i] - 'A' + 10;
    } else if (baseN[i] >= 'a' && baseN[i] <= 'z') {
      digit = baseN[i] - 'a' + 10;
    } else {
      return -1;
    }
    base10 = base10 * base + digit;
  }
  return base10;
}

void strrev(char* str) {
  int len = strlen(str);
  for (int i = len / 2 - 1; i >= 0; i--) {
    char tmp = str[i];
    str[i] = str[len - i - 1];
    str[len - i - 1] = tmp;
  }
}

char* base10ToBase(int base, int base10, char* buf) {
  int i = 0;
  while (base10) {
    int digit = base10 % base;
    if (digit < 10) {
      buf[i] = digit + '0';
    } else {
      buf[i] = digit - 10 + 'A';
    }
    base10 /= base;
    i++;
  }
  buf[i] = '\0';
  strrev(buf);
  return buf;
}

int main(int argc, char* argv[]) {
  if (argc == 2 && !strcmp(argv[argc - 1], "--run-tests")) {
    return execute_unit_tests(argc - 1, argv);
  }

  if (argc != 4) {
    usage(argv);
    return 1;
  }

  int base = atoi(argv[2]);

  if (strcmp(argv[1], "from-base") == 0) {
    int base10 = base10FromBase(base, argv[3]);
    if (base10 == -1) {
      fprintf(stderr, "Invalid digit in base %d number %s\n", base, argv[3]);
      return 1;
    }

    printf("%d\n", base10);
    return 0;
  }

  if (strcmp(argv[1], "to-base") == 0) {
    char buf[256];

    char* baseN = base10ToBase(base, atoi(argv[3]), buf);
    if (baseN == NULL) {
      fprintf(stderr, "Invalid digit in base %d number %s\n", base, argv[3]);
      return 1;
    }

    printf("%s\n", baseN);
    return 0;
  }

  usage(argv);
  return 1;
}

static MunitResult testBaseNto10() {
  munit_assert_true(base10FromBase(2, "101") == 5);
  munit_assert_true(base10FromBase(2, "1010") == 10);
  munit_assert_true(base10FromBase(10, "10") == 10);
  munit_assert_true(base10FromBase(10, "100") == 100);
  munit_assert_true(base10FromBase(16, "A") == 10);
  munit_assert_true(base10FromBase(16, "F") == 15);
  return MUNIT_OK;
}

static MunitResult testBase10toN() {
  char buf[256];
  munit_assert_string_equal(base10ToBase(2, 5, buf), "101");
  munit_assert_string_equal(base10ToBase(2, 10, buf), "1010");
  munit_assert_string_equal(base10ToBase(10, 10, buf), "10");
  munit_assert_string_equal(base10ToBase(10, 100, buf), "100");
  munit_assert_string_equal(base10ToBase(16, 10, buf), "A");
  munit_assert_string_equal(base10ToBase(16, 15, buf), "F");
  return MUNIT_OK;
}

// SECOND, compile them into AT LEAST one list of tests. OK to split to two
// lists.
static MunitTest tests[] = {
    {"/baseNto10", testBaseNto10, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/base10toN", testBase10toN, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {},
};

int execute_unit_tests(int argc, char* argv[]) {
  static const MunitSuite primary_test_suite = {
      "/primary", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE,
  };
  return munit_suite_main(&primary_test_suite, NULL, argc, argv);
}
