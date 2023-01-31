// prime_factors finds the prime factors of the given number into the given
// factors array. The returned value is the number of factors found, or -1 if an
// error occured.
int prime_factors(int num, int *dst, int dst_len) {
  // Hack to reuse our prime_factors function by allocating a tiny factors
  // buffer.
  int pfactors_tmp[1];
  int ndst = 0;
  for (int i = 2; i <= num / 2; i++) {
    // Check if i is a factor of num.
    if (num % i == 0) {
      // If it is, then we need to check if i itself is prime, since we only
      // want to return prime factors.
      // The number is not prime.
      if (prime_factors(i, pfactors_tmp, 1) == 0) {
        // No factors were found, so i is prime.
        // Do a bound check before adding the factor.
        if (ndst == dst_len) {
          return -1;
        }
        dst[ndst] = i;
        ndst++;
      }
    }
  }
  return ndst;
}
