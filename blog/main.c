#include <getopt.h>
#include <onion/onion.h>
#include <panic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strdup.h>
#include <string.h>

#include "blog/password.h"
#include "db.h"
#include "handlers.h"

static struct option flags[] = {
    {"help", no_argument, 0, 'h'},
    {"port", required_argument, 0, 'p'},
    {"database", required_argument, 0, 'd'},
    {0, 0, 0, 0},
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

  fprintf(stderr, "Listening on http://127.0.0.1:%s\n", port);

  onion_listen(o);
  onion_free(o);

  db_close();
  return 0;
}
