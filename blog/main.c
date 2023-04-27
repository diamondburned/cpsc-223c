#include <getopt.h>
/* #include <json-c/json.h> */
/* #include <onion.h> */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "handlers.h"

static struct option flags[] = {
    {"port", required_argument, 0, 'p'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0},
};

void help(char* arg0) {
  fprintf(stderr, "Usage: %s [--port 8080] [--help]\n", arg0);
}

int main(int argc, char** argv) {
  char* port = "8080";
  int opt;
  while ((opt = getopt_long(argc, argv, "p:h", flags, NULL)) != -1) {
    switch (opt) {
      case 'h': {
        help(argv[0]);
        return 0;
      }
      case 'p': {
        port = malloc(strlen(optarg) + 1);
        strcpy(port, optarg);
        break;
      }
      default: {
        fprintf(stderr, "Unknown option: %c\n", opt);
        help(argv[0]);
        return 1;
      }
    }
  }

  onion* o = onion_new(O_ONE_LOOP | O_POLL);
  onion_set_port(o, port);

  onion_url* urls = onion_root_url(o);
  onion_url_add(urls, "^/articles", route_get_articles);
  onion_url_add(urls, "^/articles/([0-9]*)$", route_get_article);

  onion_listen(o);
  onion_free(o);
  return 0;
}
