#include <stdio.h>

#include "ascii.h"
#include "munit/munit.h"

int execute_unit_tests(int argc, char *argv[]);

int main(int argc, char *argv[]) { return execute_unit_tests(argc, argv); }

// FIRST, individual tests. Note that each may test multiple cases within it
static MunitResult test_is_printable(const MunitParameter params[],
                                     void *data) {
  munit_assert_true(is_printable('a'));
  munit_assert_true(is_printable('!'));
  munit_assert_true(is_printable('~'));
  munit_assert_false(is_printable(' '));
  munit_assert_false(is_printable('\t'));
  munit_assert_false(is_printable('\n'));
  munit_assert_false(is_printable('\r'));
  munit_assert_false(is_printable('\0'));
  return MUNIT_OK;
}

static MunitResult test_print_ascii_table(const MunitParameter params[],
                                          void *data) {
  // clang-format off
  const char *expected =
    "| 0x00 - | 0x01 - | 0x02 - | 0x03 - | 0x04 - | 0x05 - | 0x06 - | 0x07 - | \n"
    "| 0x08 - | 0x09 - | 0x0A - | 0x0B - | 0x0C - | 0x0D - | 0x0E - | 0x0F - | \n"
    "| 0x10 - | 0x11 - | 0x12 - | 0x13 - | 0x14 - | 0x15 - | 0x16 - | 0x17 - | \n"
    "| 0x18 - | 0x19 - | 0x1A - | 0x1B - | 0x1C - | 0x1D - | 0x1E - | 0x1F - | \n"
    "| 0x20 - | 0x21 ! | 0x22 \" | 0x23 # | 0x24 $ | 0x25 % | 0x26 & | 0x27 ' | \n"
    "| 0x28 ( | 0x29 ) | 0x2A * | 0x2B + | 0x2C , | 0x2D - | 0x2E . | 0x2F / | \n"
    "| 0x30 0 | 0x31 1 | 0x32 2 | 0x33 3 | 0x34 4 | 0x35 5 | 0x36 6 | 0x37 7 | \n"
    "| 0x38 8 | 0x39 9 | 0x3A : | 0x3B ; | 0x3C < | 0x3D = | 0x3E > | 0x3F ? | \n"
    "| 0x40 @ | 0x41 A | 0x42 B | 0x43 C | 0x44 D | 0x45 E | 0x46 F | 0x47 G | \n"
    "| 0x48 H | 0x49 I | 0x4A J | 0x4B K | 0x4C L | 0x4D M | 0x4E N | 0x4F O | \n"
    "| 0x50 P | 0x51 Q | 0x52 R | 0x53 S | 0x54 T | 0x55 U | 0x56 V | 0x57 W | \n"
    "| 0x58 X | 0x59 Y | 0x5A Z | 0x5B [ | 0x5C \\ | 0x5D ] | 0x5E ^ | 0x5F _ | \n"
    "| 0x60 ` | 0x61 a | 0x62 b | 0x63 c | 0x64 d | 0x65 e | 0x66 f | 0x67 g | \n"
    "| 0x68 h | 0x69 i | 0x6A j | 0x6B k | 0x6C l | 0x6D m | 0x6E n | 0x6F o | \n"
    "| 0x70 p | 0x71 q | 0x72 r | 0x73 s | 0x74 t | 0x75 u | 0x76 v | 0x77 w | \n"
    "| 0x78 x | 0x79 y | 0x7A z | 0x7B { | 0x7C | | 0x7D } | 0x7E ~ | 0x7F - | \n";
  // clang-format on

  FILE *output = tmpfile();
  print_ascii_table(output);

  // Rewind and read the output file.
  fseek(output, 0, SEEK_SET);

  char buffer[4096];  // 4KB is enough, trust me bro

  size_t filesz = fread(buffer, 1, sizeof(buffer), output);
  munit_assert_false(ferror(output));
  munit_assert_size(filesz, ==, strlen(expected));

  buffer[filesz] = '\0';
  munit_assert_string_equal(buffer, expected);

  return MUNIT_OK;
}

MunitTest tests[] = {
    {"/is_printable", test_is_printable, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/print_ascii_table", test_print_ascii_table, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {},
};

static const MunitSuite primary_test_suite = {"/primary", tests, NULL, 1,
                                              MUNIT_SUITE_OPTION_NONE};

int execute_unit_tests(int argc, char *argv[]) {
  return munit_suite_main(&primary_test_suite, NULL, argc, argv);
}
