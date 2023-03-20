#include <getopt.h>
#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bottles.h"
#include "libintl.h"

void print_usage(char* arg0) { printf("Usage: %s [-i|--start] [-j|--stop] [-l|--lang] [-h|--help]\n", arg0); }

int64_t must_parse_i64(char* str) {
if (str[0] == '\0') exit(1);
char* rest;
int64_t n = strtol(str, &rest, 10);
if (rest[0] != '\0') exit(1);
return n;
}

int main(int argc, char* argv[]) {
bindtextdomain("messages", "./locales/");
textdomain("messages");
bottles_opts opts = bottles_opts_new();
static struct option long_opts[] = {
    {"start", required_argument, 0, 'i'}, {"stop", required_argument, 0, 'j'}, {"lang", required_argument, 0, 'l'}, {"help", no_argument, 0, 'h'}, {0, 0, 0, 0},
};
for (int opt; (opt = getopt_long(argc, argv, "i:j:l:h", long_opts, NULL)) != -1; true) switch (opt) {
case 'i':
opts.start = must_parse_i64(optarg);
break;
case 'j':
opts.stop = must_parse_i64(optarg);
break;
case 'l':
if (setlocale(LC_ALL, optarg) == NULL) return 1;
break;
case 'h':
print_usage(argv[0]);
return 0;
}
if (bottles_opts_validate(opts) != NULL) return 1;
bottles(opts, stdout);
return 0;
}
