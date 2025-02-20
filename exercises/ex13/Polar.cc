// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include "Polar.h"

// overrding or defining ToString function using stringstream
std::string Polar::ToString() const {
  std::stringstream ss;
  ss << "Polar (radius=)" << radius_ << ", Angle=" << angle_ << ")";
  return ss.str();
}

// define Distance
double Polar::Distance(const Polar &other) const {
  double x1 = radius_ * cos(angle_);
  double y1 = radius_ * sin(angle_);
  double x2 = other.radius_ * cos(other.angle_);
  double y2 = other.radius_ * sin(other.angle_);

  return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}
