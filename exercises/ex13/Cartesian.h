// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#ifndef _CARTESIAN_H_
#define _CARTESIAN_H_

#include <string>   // for std::string
#include <sstream>
#include "Coordinate.h"

// inherit from Coordinate class
class Cartesian : public Coordinate{
 public:
  // ctor, takes in x and y cartesian coordinates
  Cartesian(const double x, const double y) : x_(x), y_(y) {}
  ~Cartesian() = default;  // defualt destructor
  // override the pure virtual function
  std::string ToString() const override;

  // add a member function
  double Distance(const Cartesian &other) const;
 private:
  double x_, y_;
};

#endif  // _CARTESIAN_H_
