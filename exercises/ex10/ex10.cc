// name: Zongze Wu
// UW email: zongzewu@uw.edu

/*
 * Copyright ©2025 Hal Perkins.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Winter Quarter 2025 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

/* 
 * ex10.cc - test program for Vector class
 * CSE 333 exercise 9 sample solution.
 */

#include <iostream>
#include <cstdlib>

#include "Vector.h"

using std::cout;
using std::endl;
using vector333::Vector;

// Create several vectors and test their operations
int main() {
  // test constructors and stream output
  Vector v1;
  cout << "default Vector, should be (0,0,0): " << v1 << endl;
  Vector v2(1, 2, 3);
  cout << "Vector with initial values, should be (1,2,3): " << v2 << endl;
  Vector v3(v2);
  cout << "Vector from copy constructor, should be (1,2,3): " << v3 << endl;

  // test assignment (including chaining)
  Vector v4(3.1, -2.5, 2.7);
  v1 = v3 = v4;
  cout << "Vector assignment, should have three copies of (3.1,-2.5,2.7):\n";
  cout << "  " << v1 << "  " << v3 << "  " << v4 << endl;

  // test updating assignment (including chaining)
  cout << "Updating assignment, should have two copies of (4.1,-0.5,5.7):\n";
  v1 = v3 += v2;
  cout << " " << v1 << " = " <<  v3 << endl;
  cout << "Updating assignment, should have two copies of (3.1,-2.5,2.7):\n";
  v1 = v3 -= v2;
  cout << " " <<  v1 << " = " <<  v3 << endl;

  // element-wise arithmetic
  cout << "Arithmetic:" << endl;
  cout << "  " << v1 << " + " << v2 << " = " << v1+v2 << endl;
  cout << "  " << v1 << " - " << v2 << " = " << v1-v2 << endl;

  // dot product
  cout << "Dot product: (a,b,c) * (x,y,z) = (ax+by+cz)" << endl;
  cout << "  " << v2 << " * " << v2 << " = " << v2*v2 << endl;

  // scalar product
  cout << "Scalar product: (a,b,c) * k = (ak,bk,ck)" << endl;
  cout << "  "       << v1 << " * 2 = " << v1*2 << endl;
  cout << "  2 * " << v1 <<       " = " << 2*v1 << endl;

  return EXIT_SUCCESS;
}