#include "prime.h"

#include <math.h>

bool is_prime(int64_t num) {
    // 1 and 0 are not prime
    if (num < 2) {
        return false;
    }

    // fast path: even numbers are never prime except for 2
    if (num % 2 == 0) {
        return num == 2;
    }

    for (int64_t i = 3; i <= sqrt(num); i++) {
        if (num % i == 0) {
            return false;
        }
    }

    return true;
}
