// Copyright (c) 2025 Z. Wu
// Name: Zongze Wu
// UW Email: zongzewu@uw.edu

#include <iostream>
#include <sstream>
#include <string>

#include "Vector.h"

namespace vector333 {
  // default constructor
  Vector::Vector() {
    x_ = 0;
    y_ = 0;
    z_ = 0;
  }

  // constructor takes x, y, z
  Vector::Vector(const double x, const double y, const double z) :
                                          x_(x), y_(y), z_(z) {}

  // copy constructor
  Vector::Vector(const Vector& copyme) {
    x_ = copyme.x_;
    y_ = copyme.y_;
    z_ = copyme.z_;
  }

  Vector::~Vector() {}  // destructor

  Vector &Vector::operator=(const Vector &a) {
    if (this != &a) {
      x_ = a.x_;
      y_ = a.y_;
      z_ = a.z_;
    }
    return *this;
  }

  Vector &Vector::operator+=(const Vector &a) {
    x_ += a.x_;
    y_ += a.y_;
    z_ += a.z_;
    return *this;
  }

  Vector &Vector::operator-=(const Vector &a) {
    x_ -= a.x_;
    y_ -= a.y_;
    z_ -= a.z_;
    return *this;
  }

  Vector &Vector::operator*=(const double k) {
    x_ *= k;
    y_ *= k;
    z_ *= k;
    return *this;
  }

// additional non-member overloaded operators in vector333 namespace

  Vector operator+(const Vector &a, const Vector &b) {
    Vector tmp = a;
    tmp += b;
    return tmp;
  }

  Vector operator-(const Vector &a, const Vector &b) {
    Vector tmp = a;
    tmp -= b;
    return tmp;
  }

  double operator*(const Vector &a, const Vector &b) {
    double result = 0;
    result += a.get_x() * b.get_x();
    result += a.get_y() * b.get_y();
    result += a.get_z() * b.get_z();
    return result;
  }

  Vector operator*(const Vector &a, const double k) {
    Vector tmp = a;
    tmp *= k;
    return tmp;
  }

  Vector operator*(const double k, const Vector &a) {
    Vector tmp = a;
    tmp *= k;
    return tmp;
  }

  std::ostream &operator<<(std::ostream &out, const Vector &a) {
    out << "(" << a.get_x() << "," << a.get_y() << "," << a.get_z() << ")";
    return out;
  }

}  // namespace vector333
