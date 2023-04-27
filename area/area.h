#include <stdbool.h>

typedef struct {
  double x;
  double y;
} Point;

bool parse_point(const char* str, Point* out);
double area(Point p1, Point p2, Point p3);
