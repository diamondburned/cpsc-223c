#include "humanize.h"

#include <ctype.h>
#include <libintl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

const int64_t quadrillion = 1000000000000000;
const int64_t trillion = 1000000000000;
const int64_t billion = 1000000000;
const int64_t million = 1000000;
const int64_t thousand = 1000;
const int64_t hundred = 100;
const int64_t ten = 10;

static char* digits[10];
static char* tens[10];
static char* nteens[10];
static bool _initialized = false;

void init_humanize() {
 if (_initialized) return;
 _initialized = true;
 digits[0] = gettext("zero");
 digits[1] = gettext("one");
 digits[2] = gettext("two");
 digits[3] = gettext("three");
 digits[4] = gettext("four");
 digits[5] = gettext("five");
 digits[6] = gettext("six");
 digits[7] = gettext("seven");
 digits[8] = gettext("eight");
 digits[9] = gettext("nine");
 nteens[0] = gettext("ten");
 nteens[1] = gettext("eleven");
 nteens[2] = gettext("twelve");
 nteens[3] = gettext("thirteen");
 nteens[4] = gettext("fourteen");
 nteens[5] = gettext("fifteen");
 nteens[6] = gettext("sixteen");
 nteens[7] = gettext("seventeen");
 nteens[8] = gettext("eighteen");
 nteens[9] = gettext("nineteen");
 tens[2] = gettext("twenty");
 tens[3] = gettext("thirty");
 tens[4] = gettext("forty");
 tens[5] = gettext("fifty");
 tens[6] = gettext("sixty");
 tens[7] = gettext("seventy");
 tens[8] = gettext("eighty");
 tens[9] = gettext("ninety");
}

size_t sprintn(char* strptr, int64_t num) {
 init_humanize();
 char* str = strptr;
 if (num >= quadrillion) return sprintf(str, "%ld", num);
 if (num < 0) {
  str = str + sprintf(str, gettext("negative "));
  num = num * -1;
 }
 if (num >= trillion) {
  str = str + sprintn(str, num / trillion);
  str = str + sprintf(str, gettext(" trillion"));
  if ((num %= trillion) != 0) str = str + sprintf(str, " ");
 }
 if (num >= billion) {
  str = str + sprintn(str, num / billion);
  str = str + sprintf(str, gettext(" billion"));
  if ((num %= billion) != 0) str = str + sprintf(str, " ");
 }
 if (num >= million) {
  str = str + sprintn(str, num / million);
  str = str + sprintf(str, gettext(" million"));
  if ((num %= million) != 0) str = str + sprintf(str, " ");
 }
 if (num >= thousand) {
  str = str + sprintn(str, num / thousand);
  str = str + sprintf(str, gettext(" thousand"));
  if ((num %= thousand) != 0) str = str + sprintf(str, " ");
 }
 if (num >= hundred) {
  str = str + sprintf(str, gettext("%s hundred"), digits[num % thousand / 100]);
  if ((num %= hundred) != 0) str = str + sprintf(str, " ");
 }
 if (num > 0 && str > strptr && str[-1] == ' ')
  if (strcmp(strptr, gettext("negative ")) != 0) str = str + sprintf(str, gettext("and "));
 if (num >= ten) {
  if (num < 20) return (str + sprintf(str, "%s", nteens[num % ten])) - strptr;
  str = str + sprintf(str, "%s", tens[num % hundred / 10]);
  if ((num %= ten) != 0) str = str + sprintf(str, "-");
 }
 if (num > 0) {
  str = str + sprintf(str, "%s", digits[num % ten]);
 }
 return str - strptr;
}

void upper_sentence(char* str) {
 if (str[0] != '\0') str[0] = toupper(str[0]);
}
