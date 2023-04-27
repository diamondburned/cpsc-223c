#include "area.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

double area(Point p1, Point p2, Point p3) {
  // clang-format off
  // Heron's formula:
  return fabs(p1.x * (p2.y - p3.y) +
              p2.x * (p3.y - p1.y) +
              p3.x * (p1.y - p2.y)) / 2;
  // clang-format on
}

bool parse_point(const char* str, Point* out) {
  const int len = strlen(str);
  if (len < 5) {  // (0, 0)
    return false;
  }
  if (str[0] != '(' || str[len - 1] != ')') {
    // scanf doesn't check for this, so we do it here.
    return false;
  }
  return sscanf(str, "(%lf, %lf)", &out->x, &out->y) == 2;
}
