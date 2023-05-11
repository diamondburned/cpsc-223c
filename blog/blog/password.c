#include "password.h"

#include <crypt.h>
#include <errno.h>
#include <onion/codecs.h>
#include <panic.h>
#include <stdlib.h>
#include <strdup.h>
#include <string.h>
#include <sys/random.h>

char* blog_password_hash(const char* pass) {
  char salt[CRYPT_GENSALT_OUTPUT_SIZE];
  if (crypt_gensalt_rn("$2b$", 0, NULL, 0, salt, sizeof(salt)) == NULL) {
    panic("Failed to generate salt: %s", strerror(errno));
  }

  // crypt_rn triggers a stack overflow according to libasan.
  // Work around by using crypt_ra.

  struct crypt_data* data = NULL;
  int data_len = 0;

  if (crypt_ra(pass, salt, (void**)&data, &data_len) == NULL) {
    panic("Failed to hash password: %s", strerror(errno));
  }

  char* hash = strdup(data->output);

  free(data);
  return hash;
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
  struct crypt_data* data = NULL;
  int data_len = 0;

  if (crypt_ra(pass, hash, (void**)&data, &data_len) == NULL) {
    panic("Failed to hash password: %s", strerror(errno));
  }

  bool res = constant_time_compare(data->output, hash);

  free(data);
  return res;
}
