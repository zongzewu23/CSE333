// Copyright (c) 2025 Z. Wu
// Name: Zongze Wu
// UW Email: zongzewu@uw.edu

#include <stdint.h>    // For int16_t, int64_t
#include <stdio.h>     // For printf, fprintf
#include <stdlib.h>    // For EXIT_SUCCESS
#include <inttypes.h>  // For PRId64

#include "NthPrime.h"  // For NthPrime

int main(int argc, char** argv) {
  int16_t mm, dd, yy;
  int64_t MM, DD, YY;
  mm = 6;
  dd = 22;
  yy = 2004;

  MM = NthPrime(mm);
  DD = NthPrime(dd);
  YY = NthPrime(yy);

  printf("My birthdayth prime numbers are(mm,dd,yyyy):\n"
          "%" PRId64 " | %" PRId64 " | %" PRId64 "\n", MM, DD, YY);

  return EXIT_SUCCESS;
}
