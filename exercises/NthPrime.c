// Copyright (c) 2025 Z. Wu
// Name: Zongze Wu
// UW email: zongzewu@uw.edu

#include <stdint.h>   // For int16_t, int64_t
#include <stdio.h>    // For printf
#include <stdlib.h>   // For EXIT_FAILURE
#include <stdbool.h>  // For true and false

#include "NthPrime.h"

// Check if a number is a prime number
static bool IsPrime(int64_t num);


int64_t NthPrime(int16_t n) {
  if (n <= 0) {
    // return err if not a positive int
    fprintf(stderr, "n is some none-positive integer: %d\n", n);
    return EXIT_FAILURE;
  } else if (n == 1) {
    // return 2 because 2 is the first prime number
    return 2;
  }

  n--;  // count in 2 as a prime
  int64_t num = 3;  // start from the second prime

  while (num <= (INT64_MAX - 2)) {
    if (IsPrime(num)) {
      // find all prime number until we find the nth prime
      if (n == 1) {
        return num;
      }
      n--;
    }
    num += 2;
  }

  if (num == INT64_MAX) {
    // return err if out of int64_t
    fprintf(stderr, "Hit the maximum value of int64_t.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static bool IsPrime(int64_t num) {
  if (num == 0 || num == 1) {
    return false;
  } else if (num == 2) {
    return true;
  } else if (num % 2 == 0) {
    return false;
  } else {
    return true;
  }
}
