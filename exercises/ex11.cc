// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include <iostream>      // for std::cin and std::cout
#include <vector>        // for vector<>
#include <algorithm>     // for std::sort
#include <cstdlib>       // for EXIT_FAILURE

template <typename T>
T ReadValue(std::istream &in) {
  // read the value from in to T var
  T var;
  in >> var;

  // if something wrong, check what's going on
  if (!in.good()) {
    if (in.fail()) {  // failed on reading input
      std::cerr << "Invalid input! Please enter a double." << std::endl;
      exit(EXIT_FAILURE);
    } else if (in.eof()) {  // unexpected end of input, ctrl + d
      std::cerr << "Unexpected end of input!" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  // return this var
  return var;
}

int main(int argc, char** argv) {
  // initialize the vector to has a double type
  std::vector<double> vec;
  std::cout << "Enter 6 doubles:" << std::endl;
  // get 6 input from user
  for (int i = 0; i < 6; i++) {
    vec.push_back(ReadValue<double>(std::cin));
  }

  std::sort(vec.begin(), vec.end());  // sort the vector useing std::sort

  std::cout << "Your sorted doubles are:" << std::endl;
  // print out the each element in the vector by using auto & for each
  for (const auto &num : vec) {
    std::cout << num << std::endl;
  }

  return EXIT_SUCCESS;
}
