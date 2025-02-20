// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include <cstdlib>    // for EXIT_SUCCESS
#include <iostream>   // for cout

#include "Cartesian.h"
#include "Polar.h"

using std::cout;
using std::endl;

int main(int argc, char** argv) {
  // construct two coordinates for each coordinate system
  Cartesian cart1(23.3, 3.88);
  Cartesian cart2(20, 34);
  Polar pol1(15, 30.11);
  Polar pol2(1, 45);

  // Testing Cartesian coordinate system
  cout << "-----Cartesian-----" << endl;
  cout << "cart1: " << cart1.ToString() << endl;
  cout << "cart2: " << cart2.ToString() << endl;
  cout << "Distance = " << cart1.Distance(cart2) << endl;

  // Testing Cartesian coordinate system
  cout << "-----Polar-----" << endl;
  cout << "pol1: " << pol1.ToString() << endl;
  cout << "pol2: " << pol2.ToString() << endl;
  cout << "Distance = " << pol1.Distance(pol2) << endl;

  return EXIT_SUCCESS;
}
