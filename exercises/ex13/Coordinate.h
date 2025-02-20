// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#include <string>   // for std::string
#include <cmath>

class Coordinate {
 public:
    virtual std::string ToString() const = 0;  // pure virtual function
    virtual ~Coordinate() {}  // virtual destructor
};

#endif  // _COORDINATE_H_
