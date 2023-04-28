#include <getopt.h>
#include <libgen.h>
#include <onion/exportlocal.h>
#include <onion/handler.h>
#include <onion/onion.h>
#include <onion/shortcuts.h>
#include <onion/static.h>
#include <panic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strdup.h>
#include <string.h>

#include "blog/password.h"
#include "db.h"
#include "handlers.h"
#include "onion/url.h"

static struct option flags[] = {
    {"help", no_argument, 0, 'h'},
    {"port", required_argument, 0, 'p'},
    {"database", required_argument, 0, 'd'},
    {0, 0, 0, 0},
};

static const char* static_files[] = {
    // TODO: figure out why onion_handler_export_local_new doesn't work.
    "public/lib.js",
    "public/styles.css",
    NULL,
};

void help(char* arg0) {
  fprintf(stderr, "Usage: %s [options]\n", arg0);
  fprintf(stderr, "\n");
  fprintf(stderr, "Options:\n");
  for (int i = 0; flags[i].name != NULL; i++) {
    fprintf(stderr, "  -%c, --%s\n", flags[i].val, flags[i].name);
  }
}

int main(int argc, char** argv) {
  char* port = "8080";
  char* db_path = "blog.db";
  int opt;
  while ((opt = getopt_long(argc, argv, "d:p:h", flags, NULL)) != -1) {
    switch (opt) {
      case 'h': {
        help(argv[0]);
        return 0;
      }
      case 'd': {
        db_path = strdup(optarg);
        break;
      }
      case 'p': {
        port = strdup(optarg);
        break;
      }
      default: {
        fprintf(stderr, "Unknown option: %c\n", opt);
        help(argv[0]);
        return 1;
      }
    }
  }

  db_init(db_path);

  onion* o = onion_new(O_ONE_LOOP | O_POLL);
  onion_set_port(o, port);

  onion_url* urls = onion_root_url(o);
  onion_url_add(urls, "^api/articles/([0-9]*)$", route_article);
  onion_url_add(urls, "^api/articles$", route_articles);
  onion_url_add(urls, "^api/users/([^/]*)$", route_user);
  onion_url_add(urls, "^api/users$", route_users);
  onion_url_add_with_data(urls, "^$",  //
                          serve_static_file, "public/index.html", NULL);
  onion_url_add_with_data(urls, "^articles/([0-9]*)$",  //
                          serve_static_file, "public/article.html", NULL);
  for (int i = 0; static_files[i] != NULL; i++) {
    const char* path = static_files[i];
    const char* name = basename((char*)path);
    onion_url_add_with_data(urls, name, serve_static_file, (void*)path, NULL);
  }

  fprintf(stderr, "Listening on http://127.0.0.1:%s\n", port);

  onion_listen(o);
  onion_free(o);

  db_close();
  return 0;
}
