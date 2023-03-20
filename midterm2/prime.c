#include "prime.h"

#include <math.h>

bool is_prime(int64_t num) {
 if (num < 2) return false;
 for (int64_t i = 3; i <= sqrt(num); i++)
  if (num % i == 0) return false;
 return true;
}
