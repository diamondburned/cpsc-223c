#include <getopt.h>
#include <json-c/json.h>
#include <stdio.h>

#include "tengine.h"
#include "util.h"

static struct option flags[] = {
    {"help", no_argument, 0, 'h'},
    {"json", required_argument, 0, 'j'},
    {"output", required_argument, 0, 'o'},
    {"template", required_argument, 0, 't'},
    {0, 0, 0, 0},
};

void usage(const char* progname) {
    fprintf(stderr, "Usage: %s [options]\n", progname);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help\t\t\tShow this help message\n");
    fprintf(stderr, "  -j, --json\t\t\tPath to JSON file\n");
    fprintf(stderr, "  -o, --output\t\t\tPath to output file\n");
    fprintf(stderr, "  -t, --template\t\tPath to template file\n");
}

int main(int argc, char* argv[]) {
    const char* json_file = "book.json";
    const char* tmpl_file = "book.tmpl";
    const char* output_file = "book.html";
    int opt;
    while ((opt = getopt_long(argc, argv, "j:o:t:h", flags, NULL)) != -1) {
        switch (opt) {
            case 'h': {
                usage(argv[0]);
                return 0;
            }
            case 'j': {
                json_file = strdup(optarg);
                break;
            }
            case 'o': {
                output_file = strdup(optarg);
                break;
            }
            case 't': {
                tmpl_file = strdup(optarg);
                break;
            }
            default: {
                fprintf(stderr, "Unknown option: %c\n", opt);
                usage(argv[0]);
                return 1;
            }
        }
    }

    Teng_template* tmpl = teng_new_from_file(tmpl_file);
    if (tmpl == NULL) {
        fprintf(stderr, "Failed to load template file %s\n", tmpl_file);
        return 1;
    }

    json_object* json = json_object_from_file(json_file);
    if (json == NULL) {
        fprintf(stderr, "Failed to load JSON file %s\n", json_file);
        return 1;
    }

    json_object_object_foreach(json, key, val) {
        teng_define(tmpl, key, json_object_get_string(val));
    }

    if (teng_generate_file(tmpl, output_file) != 0) {
        fprintf(stderr, "Failed to generate output file %s\n", output_file);
        return 1;
    }

    return 0;
}
