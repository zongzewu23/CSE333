// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#ifndef _POLAR_H_
#define _POLAR_H_

#include <string>   // for std::string
#include <sstream>
#include "Coordinate.h"

// inherit from coodrinate class
class Polar : public Coordinate {
 public:
  // accept raius and angle parameters, and convert the angle from degrees
  // to radians
  Polar(const double radius,
    const double angle) : radius_(radius), angle_(angle * M_PI / 180.0) {}
  ~Polar() = default;  // default dtor

  // override the ToString
  std::string ToString() const override;

  // add a Distance function
  double Distance(const Polar &other) const;
 private:
  double radius_, angle_;
};

#endif  // _POLAR_H_
