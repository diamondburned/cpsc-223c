#include <math.h>
#include <stdbool.h>
#include <stdio.h>

// (BEGIN) FOR TESTS
#include "munit/munit.h"

int execute_unit_tests(int argc, char *argv[]);
// (END) FOR TESTS

int main(int argc, char *argv[]) {
  if (argc == 2 && !strcmp(argv[argc - 1], "--run-tests"))
    return execute_unit_tests(argc - 1, argv);

  fprintf(stderr, "Hello, world!\n");
  return 1;
}

int my_atoi(const char *str) {
  int result = 0;
  int sign = 1;
  if (*str == '-') {
    sign = -1;
    str++;
  }
  while (*str) {
    result = result * 10 + (*str - '0');
    str++;
  }
  return result * sign;
}

/////// BEGIN UNIT TESTING HARNESS AND TESTS ///////

static MunitResult test_my_atoi(const MunitParameter params[], void *data) {
  munit_assert_true(my_atoi("0") == 0);
  munit_assert_true(my_atoi("1") == 1);
  munit_assert_true(my_atoi("10000") == 10000);
  munit_assert_true(my_atoi("-1") == -1);
  munit_assert_true(my_atoi("-10000") == -10000);
  return MUNIT_OK;
}

// SECOND, compile them into AT LEAST one list of tests. OK to split to two
// lists.
MunitTest tests[] = {
    {"/valid-input", test_my_atoi, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {},
};

static const MunitSuite primary_test_suite = {
    "/primary", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE,
};

int execute_unit_tests(int argc, char *argv[]) {
  return munit_suite_main(&primary_test_suite, NULL, argc, argv);
}
