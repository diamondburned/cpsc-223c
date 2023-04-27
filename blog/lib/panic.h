#pragma once

#include <stdio.h>
#include <stdlib.h>

// panic with line number
#define panic(fmt, ...)                                                 \
  {                                                                     \
    fprintf(stderr, "\e[31m! panic\e[0m at %s:%d: " fmt "\n", __FILE__, \
            __LINE__, ##__VA_ARGS__);                                   \
    abort();                                                            \
  }
