#include <minctest.h>
#include <stdio.h>
#include <stdlib.h>

#include "area.h"

void test_parse_point() {
  Point p;

  lequal(parse_point("(1, 2)", &p), true);
  lfequal(p.x, 1);
  lfequal(p.y, 2);

  lequal(parse_point("(0.12, 0.34)", &p), true);
  lfequal(p.x, 0.12);
  lfequal(p.y, 0.34);

  lequal(parse_point("(1, 2", &p), false);
  lequal(parse_point("1, 2)", &p), false);
  lequal(parse_point("1, 2", &p), false);
}

void test_area() {
  lfequal(area((Point){0, 0}, (Point){1, 0}, (Point){0, 1}), 0.5);
  lfequal(area((Point){0, 0}, (Point){6, 0}, (Point){6, 4}), 12);
}

int main() {
  lrun("area/parse_point", test_parse_point);
  lrun("area/area", test_area);
  lresults();
  return lfails != 0;
}
