#include "humanize.h"

#include <ctype.h>
#include <libintl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// C doesn't have digit separators?! Wow. (Yes, I am aware that C23 is
// ongoing, but come on, it took them 30 years to get to C11.)
const int64_t QUADRILLION = 1000000000000000;
const int64_t TRILLION = 1000000000000;
const int64_t BILLION = 1000000000;
const int64_t MILLION = 1000000;
const int64_t THOUSAND = 1000;
const int64_t HUNDRED = 100;
const int64_t TEN = 10;

static char* digits[10];
static char* tens[10];
static char* nteens[10];
static bool initialized = false;

void init_humanize() {
    if (initialized) {
        return;
    }

    initialized = true;

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

    if (num >= QUADRILLION) {
        // too big, just cop out
        return sprintf(str, "%ld", num);
    }

    if (num < 0) {
        str += sprintf(str, gettext("negative "));
        num = -num;
    }

    if (num >= TRILLION) {
        // one hundred and twenty-three trillion
        str += sprintn(str, num / TRILLION);
        str += sprintf(str, gettext(" trillion"));
        num %= TRILLION;
        if (num != 0) {
            str += sprintf(str, " ");
        }
    }

    if (num >= BILLION) {
        // four hundred and fifty-six billion
        str += sprintn(str, num / BILLION);
        str += sprintf(str, gettext(" billion"));
        num %= BILLION;
        if (num != 0) {
            str += sprintf(str, " ");
        }
    }

    if (num >= MILLION) {
        // seven hundred and eighty-nine million
        str += sprintn(str, num / MILLION);
        str += sprintf(str, gettext(" million"));
        num %= MILLION;
        if (num != 0) {
            str += sprintf(str, " ");
        }
    }

    if (num >= THOUSAND) {
        // one hundred and twenty-three thousand
        str += sprintn(str, num / THOUSAND);
        str += sprintf(str, gettext(" thousand"));
        num %= THOUSAND;
        if (num != 0) {
            str += sprintf(str, " ");
        }
    }

    // now we're down to the hundreds, print something like "one hundred and
    // twenty three" or "eighty nine" or "one hundred" or "one"

    if (num >= HUNDRED) {
        // x hundred
        str +=
            sprintf(str, gettext("%s hundred"), digits[num % THOUSAND / 100]);
        num %= HUNDRED;
        if (num != 0) {
            str += sprintf(str, " ");
        }
    }

    // Hack: if there's a space at the end of the string, then it probably
    // means we've printed something higher than a hundred, and we need to
    // add "and" to the end. Except when we've only printed "negative".
    if (num > 0 && str > strptr && str[-1] == ' ') {
        if (strcmp(strptr, gettext("negative ")) != 0) {
            str += sprintf(str, gettext("and "));
        }
    }

    if (num >= TEN) {
        if (num < 20) {
            str += sprintf(str, "%s", nteens[num % TEN]);
            return str - strptr;
        }

        str += sprintf(str, "%s", tens[num % HUNDRED / 10]);

        num %= TEN;
        if (num != 0) {
            str += sprintf(str, "-");
        }
    }

    if (num > 0) {
        str += sprintf(str, "%s", digits[num % TEN]);
    }

    return str - strptr;
}

void upper_sentence(char* str) {
    if (str[0] != '\0') {
        str[0] = toupper(str[0]);
    }
}
