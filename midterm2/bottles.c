#include "bottles.h"

#include <libintl.h>
#include <stdio.h>

#include "humanize.h"
#include "prime.h"

bottles_opts bottles_opts_new() { return (bottles_opts){.start = 99, .stop = 0}; }

char* bottles_opts_validate(bottles_opts opts) {
 if (opts.start < 1)
  return "start must be one or higher.";
 else if (opts.stop < 0)
  return "stop must be zero or higher.";
 else if (opts.start <= opts.stop)
  return "start must be greater than stop.";
 else
  return NULL;
}

void bottles(bottles_opts opts, FILE* out) {
 int64_t bottle = opts.start;
 char strbuf[1024];
 while (bottle != 0) {
  sprintn(strbuf, bottle);
  upper_sentence(strbuf);
  fprintf(out, gettext("%s %s of beer on the wall.\n"), strbuf, bottle == 1 ? gettext("bottle") : gettext("bottles"));
  fprintf(out, gettext("%s %s of beer.\n"), strbuf, bottle == 1 ? gettext("bottle") : gettext("bottles"));
  if (bottle == opts.stop) break;
  bottle--;
  if (is_prime(bottle))
   // The joke follows once the bottle count is prime.
   // It implies that we always pass the "prime" bottle to the
   // mathematician.
   fprintf(out, gettext("Take one down, pass it to the mathematician.\n"));
  else
   fprintf(out, gettext("Take one down, pass it around.\n"));
  if (bottle == 0) fprintf(out, "%s%s", gettext("No more bottles of beer on the wall, no more bottles of beer.\n"), gettext("Go to the store and buy some more, ninety-nine bottles of beer on the wall.\n"));
 }
}
