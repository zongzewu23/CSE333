// Copyright ©2025 Hal Perkins
// Name: Zongze Wu
// UW Email: zongzewu@uw.edu


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

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <memory>  // for std::unique_ptr

int main(int argc, char **argv) {
  // Allocate an integer on the heap, initialize to value 5.
  std::unique_ptr<int> x(new int(5));  // wrap the new int into unique_ptr
  //  int *x = new int(5);
  std::cout << "*x is: " << *x << std::endl;

  // Allocate a vector of integers on the heap, add some values to
  // that vector, sort the vector, print the values.

  // allocate a vector<int> on the heap, and let the unique_ptr take its
  // ownership
  std::unique_ptr<std::vector<int>> v(new std::vector<int>);
  //  std::vector<int> *v = new std::vector<int>;
  v->push_back(13);
  v->push_back(42);
  v->push_back(17);
  std::sort(v->begin(), v->end());
  std::cout << "sorted v: ";
  for (int &el : *v) {
    std::cout << el << " ";
  }
  std::cout << std::endl;

  // Allocate a vector of (integer pointers) on the stack, add some
  // values to the vector from the heap, print the values.
  //  std::vector<int*> v2;
  // allocate a vector on the stack that takes unique_ptr<int> as element type
  std::vector<std::unique_ptr<int>> v2;

  // allocate integers on the heap, let the unique_ptrs take the ownerships
  // then push it into the vector who takes unique_ptr as element
  v2.push_back(std::unique_ptr<int> (new int(13)));
  v2.push_back(std::unique_ptr<int> (new int(42)));
  v2.push_back(std::unique_ptr<int> (new int(17)));
  std::cout << "unsorted v2: ";
  // use const std::unique_ptr<int>& to avoid copying elements(unique_ptr)
  for (const std::unique_ptr<int>& el : v2) {
    std::cout << *el << " ";
  }
  std::cout << std::endl;

  return EXIT_SUCCESS;
  // after this scope ends, all of the unique_ptr will automatically delete
  // their loads
}
