#include <stdlib.h>

void free2_owned(void** ptr) {
  if (*ptr != NULL) {
    free(*ptr);
    *ptr = NULL;
  }
}
