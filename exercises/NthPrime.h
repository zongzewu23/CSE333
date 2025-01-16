// Copyright (c) 2025 Z. Wu
// Name: Zongze Wu
// UW Email: zongzewu@uw.edu

#ifndef NTHPRIME_H_
#define NTHPRIME_H_

#include <stdint.h>  // for int64_t, int16_t

// calculate the nth prime number
//
// Arguments:
// - n: a positive number that is less than 32767
//
// Returns a int64_t number that is the nth prime number
int64_t NthPrime(int16_t n);

#endif  // NTHPRIME_H_
