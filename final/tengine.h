#pragma once

typedef struct variable_value {
    char *varname;
    char *value;
} Teng_setting;

typedef struct template_def {
    Teng_setting *defines;
    int defines_count;

    // the text read in from the file
    char *templ;
} Teng_template;

Teng_template *teng_new_from_file(const char *template_filename);

Teng_template *teng_new_from_string(const char *templ);

char *teng_escape_html(const char *original);

void teng_define(Teng_template *tmpl, const char *varname, const char *value);

char *teng_generate(Teng_template *tmpl);

// teng_generate_file returns 0 if successful, 1 if file cannot be opened.
int teng_generate_file(Teng_template *tmpl, const char *filename);

void teng_free(Teng_template *tmpl);
