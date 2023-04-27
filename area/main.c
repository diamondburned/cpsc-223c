#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "area.h"

int main() {
  Point pts[3];
  char buf[128];
  for (int i = 0; i < 3; i++) {
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
      perror("fgets");
      return 1;
    }

    // remove trailing newline
    buf[strcspn(buf, "\n")] = 0;

    if (!parse_point(buf, &pts[i])) {
      fprintf(stderr, "invalid point: %s", buf);
      return 1;
    }
  }

  printf("%.2lf\n", area(pts[0], pts[1], pts[2]));
  return 0;
}
