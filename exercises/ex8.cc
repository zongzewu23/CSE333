// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu


#include <iostream>   // for std::cout std::cin
#include <cstdlib>    // for EXIT_SUCCESS
#include <string>     // for string
using namespace std;

// print num's factors to the stdout in ascending order
void printFactors(int num);

int main(int argc, char** argv) {
  int num;
  string prompt("Which positive integer would you like me to factorize? ");
  cout << prompt;  // print prompt
  cin >> num;  // read input
  // failed on reading input
  if (cin.fail()) {
    cerr << "Invalid input! Please enter a positive integer." << endl;
    return EXIT_FAILURE;
  }
  // input contains non integer chars
  if (cin.peek() != '\n') {
    cerr << "Invalid input! Only Positive Integer is accepted." << endl;
    return EXIT_FAILURE;
  }
  // input is a non-positive integer
  if (num <= 0) {
    cerr << "Must enter a positive integer!" << endl;
    return EXIT_FAILURE;
  }

  // print factors
  printFactors(num);

  return EXIT_SUCCESS;
}

void printFactors(int num) {
  // brute force
  for (int i = 1; i <= num; i++) {
    // factors except num itself
    if (num % i == 0 && i != num) {
      cout << i << " ";
    // num itself and end the line and flush the buffer
    } else if (num % i == 0 && i == num) {
      cout << i << endl;
    }
  }
}
