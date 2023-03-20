#include <getopt.h>
#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bottles.h"
#include "libintl.h"

void print_usage(char* arg0) {
    printf("Usage: %s [-i|--start] [-j|--stop] [-l|--lang] [-h|--help]\n",
           arg0);
}

int64_t must_parse_i64(char* str) {
    if (str[0] == '\0') {
        fprintf(stderr, "Error: number cannot be empty string\n");
        exit(1);
    }

    char* rest;
    int64_t n = strtol(str, &rest, 10);
    // Documentation writes that if str[0] is not null but rest[0] is, then
    // string is valid. We check for null and return invalid.
    if (rest[0] != '\0') {
        fprintf(stderr, "Error: invalid number: %s\n", str);
        exit(1);
    }

    return n;
}

// I will happily accept my points being docked off for using UTF-8 for
// localization than using anything else. ISO-LATIN? bruh.

int main(int argc, char* argv[]) {
    // Initialize locales.
    bindtextdomain("messages", "./locales/");
    textdomain("messages");

    bottles_opts opts = bottles_opts_new();

    static struct option long_opts[] = {
        {"start", required_argument, 0, 'i'},
        {"stop", required_argument, 0, 'j'},
        {"lang", required_argument, 0, 'l'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0},
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "i:j:l:h", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'i': {
                opts.start = must_parse_i64(optarg);
                break;
            }
            case 'j': {
                opts.stop = must_parse_i64(optarg);
                break;
            }
            case 'l': {
                if (setlocale(LC_ALL, optarg) == NULL) {
                    fprintf(stderr,
                            "Error: invalid locale %s "
                            "(did you include the country? try en_US/vi_VN)\n",
                            optarg);
                    return 1;
                }
                break;
            }
            case 'h': {
                print_usage(argv[0]);
                return 0;
            }
            default: {
            }
        }
    }

    char* err = bottles_opts_validate(opts);
    if (err != NULL) {
        printf("Invalid options: %s\n", err);
        return 1;
    }

    bottles(opts, stdout);
    return 0;
}
