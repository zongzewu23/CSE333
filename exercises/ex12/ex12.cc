// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include <fstream>   // for std::ifstream
#include <iostream>  // for cout
#include <cstdlib>   // for EXIT_SUCCESS EXIT_FAILURE
#include <string>    // for string
#include <map>       // for map

using namespace std;

// Print usage message and exit
void Usage() {
  cerr << "Usage: ./ex11 filename" << endl;
  exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    Usage();
  }
  // Use the STL map to store the unique key and value
  map<string, int> pairs;
  // open the file whose file name is the second input parameter
  // using the ifstream, the inputFile is a variable returned by
  // the ifstream ctor
  ifstream inputFile(argv[1]);
  // check if the file was successfully opened
  if (!inputFile.is_open()) {
    cerr << "Failed open file" << endl;
    return EXIT_FAILURE;
  }

  string word;  // store the read word by >>
  while (inputFile >> word) {
    // depending on whether the key already exits,
    // either construct a new map<string, int> pair, or increment the
    // value by one
    pairs[word] += 1;
  }

  // using foreach and auto& pair to iterate through the map and out put
  // the key and it's value
  for (const auto & pair : pairs) {
    cout << pair.first << " " << pair.second << endl;
  }

  inputFile.close();  // no need to explictly call, but call it here

  return EXIT_SUCCESS;
}

