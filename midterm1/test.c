#include <minctest.h>
#include <stdio.h>

#include "bottles.h"
#include "humanize.h"
#include "prime.h"

void test_sprintn() {
    char buf[1024];

    sprintn(buf, 1234567890);
    lsequal(buf,
            "one billion two hundred and thirty-four million five hundred and "
            "sixty-seven thousand eight hundred and ninety");

    sprintn(buf, 123456789);
    lsequal(buf,
            "one hundred and twenty-three million four hundred and fifty-six "
            "thousand seven hundred and eighty-nine");

    sprintn(buf, 1000000000000);
    lsequal(buf, "one trillion");

    sprintn(buf, 1000000000000000);
    lsequal(buf, "1000000000000000");

    sprintn(buf, 100010);
    lsequal(buf, "one hundred thousand and ten");

    sprintn(buf, 100011);
    lsequal(buf, "one hundred thousand and eleven");

    sprintn(buf, 100012);
    lsequal(buf, "one hundred thousand and twelve");

    sprintn(buf, 100013);
    lsequal(buf, "one hundred thousand and thirteen");

    sprintn(buf, 42);
    lsequal(buf, "forty-two");

    sprintn(buf, -1);
    lsequal(buf, "negative one");
}

void test_is_prime() {
    lequal(is_prime(0), false);
    lequal(is_prime(1), false);
    lequal(is_prime(2), true);
    lequal(is_prime(3), true);
    lequal(is_prime(4), false);
    lequal(is_prime(5), true);
    lequal(is_prime(6), false);
    lequal(is_prime(7), true);
    lequal(is_prime(8), false);
    lequal(is_prime(9), false);
    lequal(is_prime(10), false);
    lequal(is_prime(11), true);
    lequal(is_prime(12), false);
    lequal(is_prime(13), true);
}

void test_bottles() {
    FILE* tmpf = tmpfile();
    char buf[1024];

    fseek(tmpf, 0, SEEK_SET);
    bottles((bottles_opts){.start = 3, .stop = 2}, tmpf);

    fseek(tmpf, 0, SEEK_SET);
    buf[fread(buf, 1, 1024, tmpf)] = '\0';

    lsequal(buf,
            "Three bottles of beer on the wall.\n"
            "Three bottles of beer.\n"
            "Take one down, pass it to the mathematician.\n"
            "Two bottles of beer on the wall.\n"
            "Two bottles of beer.\n");

    fseek(tmpf, 0, SEEK_SET);
    bottles((bottles_opts){.start = 1, .stop = 0}, tmpf);

    fseek(tmpf, 0, SEEK_SET);
    buf[fread(buf, 1, 1024, tmpf)] = '\0';

    lsequal(buf,
            "One bottle of beer on the wall.\n"
            "One bottle of beer.\n"
            "Take one down, pass it around.\n"
            "No more bottles of beer on the wall, no more bottles of beer.\n"
            "Go to the store and buy some more, ninety-nine bottles of beer on "
            "the wall.\n");
}

int main() {
    lrun("humanize/sprintn", test_sprintn);
    lrun("prime/is_prime", test_is_prime);
    lrun("bottles/bottles", test_bottles);
    lresults();
    return lfails != 0;
}
