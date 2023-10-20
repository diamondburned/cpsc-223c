#include <minctest.h>
#include <stdbool.h>
#include <stdlib.h>

#include "tengine.h"

void test_example() {
    Teng_template *t = teng_new_from_string(
        "<html><head><title>${page_title}</title></head>\n"
        "<body>\n"
        "<p>${page_head}</p>\n"
        "<div>${page_text}</div>\n"
        "<h4>${footer}</h4>\n"
        "</body>\n"
        "</html>\n");

    teng_define(t, "footer", "Page 1 of 97");
    teng_define(t, "page_head", "Words of Wisdom");
    teng_define(t, "page_title", "Experiences from 223C");
    teng_define(
        t, "page_text",
        "It was the best of classes, it was the worst of classes. The professor was nice, but a "
        "bit insane in what he expected. For example, he asked people to actually write code and "
        "tests! For that reason, I am giving him a 0.0 on the SOQ. However, he did bring cookies.");

    char *result = teng_generate(t);
    lsequal(
        "<html><head><title>Experiences from 223C</title></head>\n"
        "<body>\n"
        "<p>Words of Wisdom</p>\n"
        "<div>It was the best of classes, it was the worst of classes. The professor was nice, but "
        "a bit insane in what he expected. For example, he asked people to actually write code and "
        "tests! For that reason, I am giving him a 0.0 on the SOQ. However, he did bring "
        "cookies.</div>\n"
        "<h4>Page 1 of 97</h4>\n"
        "</body>\n"
        "</html>\n",
        result);

    free(result);
    teng_free(t);
}

bool teng_is_valid(const char *templ) {
    Teng_template *t = teng_new_from_string(templ);
    if (t == NULL) {
        return false;
    }
    teng_free(t);
    return true;
}

void test_passing_cases() {
    lok(teng_is_valid("Hello, ${name}! ${name}!"));
    lok(teng_is_valid("${x}"));
    lok(teng_is_valid("${}"));   // yes, this is valid!
    lok(teng_is_valid(""));      // I mean, it's just an empty string
    lok(teng_is_valid("${"));    // parsed as a literal
    lok(teng_is_valid("}"));     // also parsed as a literal
    lok(teng_is_valid("${{}"));  // I don't know but this looks valid to me lol
    lok(teng_is_valid("${}}"));  // looks good to me!
}

void test_html_escape() {
    char *escaped;

    escaped = teng_escape_html("<script>alert('hi');</script>");
    lsequal("&lt;script&gt;alert(&apos;hi&apos;);&lt;/script&gt;", escaped);
    free(escaped);

    escaped = teng_escape_html("Hello, world!");
    lsequal("Hello, world!", escaped);
    free(escaped);

    escaped = teng_escape_html("Hello, \"world\"!");
    lsequal("Hello, &quot;world&quot;!", escaped);
    free(escaped);

    escaped = teng_escape_html("<<<<<<");
    lsequal("&lt;&lt;&lt;&lt;&lt;&lt;", escaped);
    free(escaped);
}

int main() {
    lrun("example", test_example);
    lrun("html_escape", test_html_escape);
    lrun("passing_cases", test_passing_cases);
    lresults();
    return lfails != 0;
}
