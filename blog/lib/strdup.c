#include "strdup.h"

#include <panic.h>
#include <stdlib.h>

char* strdup(const char* s) {
  if (s == NULL) {
    return NULL;
  }
  const size_t len = strlen(s);
  char* ret = malloc(len + 1);
  memcpy(ret, s, len + 1);
  return ret;
}

uint8_t* memdup(const uint8_t* s, size_t len) {
  uint8_t* ret = malloc(len);
  memcpy(ret, s, len);
  return ret;
}
