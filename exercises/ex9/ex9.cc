// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include <iostream>  // for ostream
#include <cstdlib>   // for EXIT_SUCCESS
#include <string>    // for string

#include "Vector.h"

int main(int argc, char** argv) {
  vector333::Vector dft;  // default construct
  vector333::Vector a(1, 2, 3), b(5, 5, 5);
  vector333::Vector c(a);  // copy construct
  vector333::Vector d = b + a;  // copy constructor and vector333 +

  std::cout << "Testing default ctors, ctors, and copy ctor: " << std::endl;
  std::cout << "default vector = " << dft << "\n" << "a = " << a << "\n"
  << "b = " << b << "\n" << "c = " << c << "\n" << "d = " << d << std::endl;

  a = d;
  b += c;
  d -= c;

  std::cout << "Testing =, +=, -=: " << std::endl;
  std::cout << "a = d, a = " << a << "\n" << "b += c, b = " << b << "\n"
    << "d -= c, d = "<< d << std::endl;

  double dotProduct = c * b;
  double k = 5;
  d = a * k;
  b = k * a;

  std::cout << "Testing dot product, and scale multiplication: " << std::endl;
  std::cout << "BTW, k = 5" << std::endl;
  std::cout << "c * b = " << dotProduct << "\n" << "d = a * k = " << d << "\n"
    << "b = k * a = "<< b << std::endl;

  a = a + b;
  d = d - b;

  std::cout << "Testing +, - : " << std::endl;
  std::cout << "a = a + b = " << a << "\n" << "d = d - b = " << d << std::endl;

  return EXIT_SUCCESS;
}
