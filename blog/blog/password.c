#include "password.h"

#include <crypt.h>
#include <onion/codecs.h>
#include <panic.h>
#include <stdlib.h>
#include <strdup.h>
#include <string.h>
#include <sys/random.h>

// transfer: full
char* generate_salt() {
  const char* prefix = "$6$";  // GNU extension for SHA-512
  const int randbuf_size = 8;

  uint8_t rand_buf[8];
  int rc = getrandom(rand_buf, sizeof(rand_buf), 0);
  if (rc == -1) {
    panic("Failed to generate salt");
  }

  char* encoded_buf = onion_base64_encode((const char*)&rand_buf, randbuf_size);

  char* salt = malloc(strlen(prefix) + strlen(encoded_buf) + 1);

  char* n = salt;
  n = strcpy(n, prefix);
  n = strcpy(n + strlen(prefix), encoded_buf);

  free(encoded_buf);
  return salt;
}

char* blog_password_hash(const char* pass) {
  // Use GNU extension for a SHA-512 hash.
  char* salt = generate_salt();
  char* hash = crypt(pass, salt);
  free(salt);

  // hash is valid until the next call to crypt().
  // We'll allocate a copy of it so we can free it later.
  return strdup(hash);
}

bool constant_time_compare(const char* a, const char* b) {
  char result = 0;
  size_t length_a = strlen(a);
  size_t length_b = strlen(b);
  if (length_a != length_b) {
    // developer's fault, so panic
    panic("constant_time_compare: strings are not of equal length");
  }
  for (size_t i = 0; i < length_a; i++) {
    result |= a[i] ^ b[i];
  }
  return result == 0;
}

bool blog_password_verify(const char* pass, const char* hash) {
  char* new_hash = crypt(pass, hash);
  return constant_time_compare(new_hash, hash);
}
