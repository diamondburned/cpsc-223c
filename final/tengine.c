#include "tengine.h"

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

const char *TENG_REGEXP = "\\$\\{([^}]*)\\}";

Teng_template *teng_new_from_string_internal(char *templ) {
    Teng_template *tmpl = malloc(sizeof(Teng_template));
    *tmpl = (Teng_template){
        .defines_count = 0,
        .defines = NULL,
        .templ = templ,
    };
    return tmpl;
}

Teng_template *teng_new_from_file(const char *template_filename) {
    FILE *f = fopen(template_filename, "r");
    if (f == NULL) {
        return NULL;
    }

    if (fseek(f, 0, SEEK_END) == -1) {
        fclose(f);
        return NULL;
    }

    size_t size = ftell(f);
    rewind(f);

    char *buffer = malloc(size + 1);

    size_t read = fread(buffer, 1, size, f);
    if (read != size) {
        fclose(f);
        free(buffer);
        return NULL;
    }

    fclose(f);
    buffer[size] = '\0';

    return teng_new_from_string_internal(buffer);
}

Teng_template *teng_new_from_string(const char *templ) {
    char *copy = malloc(strlen(templ) + 1);
    strcpy(copy, templ);
    return teng_new_from_string_internal(copy);
}

const char *teng_escape_html_char(char c) {
    switch (c) {
        case '<':
            return "&lt;";
        case '>':
            return "&gt;";
        case '&':
            return "&amp;";
        case '"':
            return "&quot;";
        case '\'':
            return "&apos;";
        default:
            return NULL;
    }
}

char *teng_escape_html(const char *original) {
    size_t old_size = strlen(original);
    size_t new_size = 0;
    for (size_t i = 0; i < old_size; i++) {
        const char *replacement = teng_escape_html_char(original[i]);
        if (replacement != NULL) {
            new_size += strlen(replacement);
        } else {
            new_size++;
        }
    }

    char *safe = calloc(new_size + 1, 1);
    char *cursor = safe;
    for (size_t i = 0; i < old_size; i++) {
        const char *replacement = teng_escape_html_char(original[i]);
        if (replacement != NULL) {
            strappend(&cursor, replacement);
            continue;
        }
        cursor[0] = original[i];
        cursor++;
    }
    cursor[0] = '\0';
    return safe;
}

void teng_define(Teng_template *tmpl, const char *varname, const char *value) {
    for (int i = 0; i < tmpl->defines_count; i++) {
        if (strcmp(tmpl->defines[i].varname, varname) == 0) {
            tmpl->defines[i].value = strdup(value);
            return;
        }
    }

    tmpl->defines_count++;
    tmpl->defines = realloc(tmpl->defines, tmpl->defines_count * sizeof(Teng_setting));
    tmpl->defines[tmpl->defines_count - 1] = (Teng_setting){
        .varname = strdup(varname),
        .value = teng_escape_html(value),
    };
}

const Teng_setting *teng_find_defn(Teng_template *tmpl, const char *varname, int varname_len) {
    for (int i = 0; i < tmpl->defines_count; i++) {
        if (strncmp(tmpl->defines[i].varname, varname, varname_len) == 0) {
            return &tmpl->defines[i];
        }
    }
    return NULL;
}

typedef struct {
    regoff_t start;
    regoff_t end;
    const Teng_setting *define;
} Teng_match;

char *teng_generate(Teng_template *tmpl) {
    static regex_t *regex;
    if (regex == NULL) {
        regex = malloc(sizeof(regex_t));
        int res = regcomp(regex, TENG_REGEXP, REG_EXTENDED);
        if (res != 0) {
            char error[1024];
            regerror(res, regex, error, 1024);
            fprintf(stderr, "BUG: regex error: %s\n", error);
            abort();  // unreachable
        }
    }

    int match_count = 0;
    size_t matching_cursor = 0;
    size_t tmpl_len = strlen(tmpl->templ);

    while (matching_cursor < tmpl_len) {
        regmatch_t matches[1];
        if (regexec(regex, &tmpl->templ[matching_cursor], 1, matches, 0) != 0) {
            break;
        }

        matching_cursor += matches[0].rm_eo;
        match_count++;
    }

    Teng_match *all_matches = calloc(match_count, sizeof(Teng_match));
    matching_cursor = 0;

    for (int i = 0; i < match_count; i++) {
        regmatch_t matches[2];
        if (regexec(regex, &tmpl->templ[matching_cursor], 2, matches, 0) != 0) {
            abort();  // unreachable
        }

        const char *varname = &tmpl->templ[matching_cursor + matches[1].rm_so];
        const int varname_len = matches[1].rm_eo - matches[1].rm_so;

        const Teng_setting *define = teng_find_defn(tmpl, varname, varname_len);
        if (define != NULL) {
            all_matches[i] = (Teng_match){
                .start = matching_cursor + matches[0].rm_so,
                .end = matching_cursor + matches[0].rm_eo,
                .define = define,
            };
        }

        matching_cursor += matches[0].rm_eo;
    }

    // Self-note:
    // For string "abc${def}ghi" where def is defined as "12345"

    size_t output_len = 0;
    size_t prev_end = 0;

    for (int i = 0; i < match_count; i++) {
        Teng_match match = all_matches[i];
        if (match.define == NULL) {
            continue;
        }

        output_len += match.start - prev_end;       // "abc"
        output_len += strlen(match.define->value);  // "12345"
        prev_end = match.end;
    }

    output_len += tmpl_len - prev_end;  // "ghi"

    char *output = calloc(output_len + 1, 1);
    char *out_cursor = output;
    prev_end = 0;

    for (int i = 0; i < match_count; i++) {
        Teng_match match = all_matches[i];
        if (match.define == NULL) {
            continue;
        }

        strnappend(&out_cursor, &tmpl->templ[prev_end], match.start - prev_end);  // "abc"
        strnappend(&out_cursor, match.define->value,
                   strlen(match.define->value));  // "12345"
        prev_end = match.end;
    }

    strnappend(&out_cursor, &tmpl->templ[prev_end], tmpl_len - prev_end);  // "ghi"
    out_cursor[0] = '\0';

    free(all_matches);
    return output;
}

// return 0 if successful, 1 if file cannot be opened
int teng_generate_file(Teng_template *tmpl, const char *filename) {
    FILE *f = fopen(filename, "w+");
    if (f == NULL) {
        return 1;
    }

    char *output = teng_generate(tmpl);
    fputs(output, f);
    free(output);

    fclose(f);
    return 0;
}

void teng_free(Teng_template *tmpl) {
    for (int i = 0; i < tmpl->defines_count; i++) {
        free(tmpl->defines[i].varname);
        free(tmpl->defines[i].value);
    }
    free(tmpl->defines);
    free(tmpl->templ);
    free(tmpl);
}
