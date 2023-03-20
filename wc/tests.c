#include <stdio.h>

#include "munit/munit.h"
#include "wc.h"

static MunitResult test_wc_str() {
  wc_results results = {};
  wc_flags flags = {
      .count_frequency = true,
      .max_line_length = true,
  };

  wc_str("hello world", WC_WORDS, flags, &results);
  munit_assert_int(results.count, ==, 2);
  munit_assert_int(results.max_line_length, ==, 11);
  munit_assert_int(results.count_frequency['h'], ==, 1);
  munit_assert_int(results.count_frequency['e'], ==, 1);
  munit_assert_int(results.count_frequency['l'], ==, 3);
  munit_assert_int(results.count_frequency['o'], ==, 2);
  munit_assert_int(results.count_frequency[' '], ==, 1);
  munit_assert_int(results.count_frequency['w'], ==, 1);
  munit_assert_int(results.count_frequency['r'], ==, 1);
  munit_assert_int(results.count_frequency['d'], ==, 1);

  results = (wc_results){};
  wc_str("  \t hello \n\n ", WC_WORDS, flags, &results);
  munit_assert_int(results.count, ==, 1);

  results = (wc_results){};
  wc_str(" ", WC_WORDS, flags, &results);
  munit_assert_int(results.count, ==, 0);

  results = (wc_results){};
  wc_str(" \n", WC_WORDS, flags, &results);
  munit_assert_int(results.count, ==, 0);

  results = (wc_results){};
  wc_str("\n ", WC_WORDS, flags, &results);
  munit_assert_int(results.count, ==, 0);

  results = (wc_results){};
  wc_str("hello\nworld\n!!!!!!", WC_LINES, flags, &results);
  munit_assert_int(results.count, ==, 3);
  munit_assert_int(results.max_line_length, ==, 6);

  results = (wc_results){};
  wc_str("\x01\x01\x01", WC_BYTES, flags, &results);
  munit_assert_int(results.count, ==, 3);

  return MUNIT_OK;
}

static MunitResult test_wc() {
  wc_results results = {};
  wc_flags flags = {
      .count_frequency = true,
      .max_line_length = true,
  };

  FILE *tmp = tmpfile();
  fputs("hello world\n", tmp);
  fseek(tmp, 0, SEEK_SET);

  results = (wc_results){};
  wc(tmp, WC_WORDS, flags, &results);
  munit_assert_int(results.count, ==, 2);
  munit_assert_int(results.max_line_length, ==, 11);
  munit_assert_int(results.count_frequency['h'], ==, 1);
  munit_assert_int(results.count_frequency['e'], ==, 1);
  munit_assert_int(results.count_frequency['l'], ==, 3);
  munit_assert_int(results.count_frequency['o'], ==, 2);
  munit_assert_int(results.count_frequency[' '], ==, 1);
  munit_assert_int(results.count_frequency['w'], ==, 1);
  munit_assert_int(results.count_frequency['r'], ==, 1);
  munit_assert_int(results.count_frequency['d'], ==, 1);

  fclose(tmp);
  return MUNIT_OK;
}

MunitTest tests[] = {
    {"/wc_str", test_wc_str, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/wc", test_wc, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {},
};

static const MunitSuite primary_test_suite = {"/primary", tests, NULL, 1,
                                              MUNIT_SUITE_OPTION_NONE};

int execute_unit_tests(int argc, char *argv[]) {
  return munit_suite_main(&primary_test_suite, NULL, argc, argv);
}

int main(int argc, char *argv[]) { return execute_unit_tests(argc, argv); }
