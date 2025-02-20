// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include "Cartesian.h"

// implement ToSting function using stringstream
std::string Cartesian::ToString() const {
  std::stringstream ss;
  ss << "Cartesian (x=" << x_ << ", y=" << y_ << ")";
  return ss.str();
}

// implement Distance
double Cartesian::Distance(const Cartesian & other) const {
  return std::sqrt(std::pow(x_ - other.x_, 2) + std::pow(y_ - other.y_, 2));
}
