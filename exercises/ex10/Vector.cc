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
    data = new double[3]{0,0,0};
  }

  // constructor takes x, y, z
  Vector::Vector(const double x, const double y, const double z) {
    data = new double[3]{x,y,z};
  }

  // copy constructor
  Vector::Vector(const Vector& copyme) {
    data = new double[3]{copyme.data[0],copyme.data[1],copyme.data[2]};
  }

  Vector::~Vector() {
    delete[] data;  // this is an array, don't forget the []
  }  // destructor

  Vector &Vector::operator=(const Vector &a) {
    if (this != &a) {
      data[0] = a.data[0];
      data[1] = a.data[1];
      data[2] = a.data[2];
    }
    return *this;
  }

  Vector &Vector::operator+=(const Vector &a) {
    data[0] += a.data[0];
    data[1] += a.data[1];
    data[2] += a.data[2];
    return *this;
  }

  Vector &Vector::operator-=(const Vector &a) {
    data[0] -= a.data[0];
    data[1] -= a.data[1];
    data[2] -= a.data[2];
    return *this;
  }

// additional non-member overloaded operators in vector333 namespace

  Vector operator+(const Vector &a, const Vector &b) {
    return Vector(a.data[0]+b.data[0],a.data[1]+b.data[1],a.data[2]+b.data[2]);
  }

  // another way to do this, still rely on "friend" cause it calls the member
  Vector operator-(const Vector &a, const Vector &b) {
    Vector tmp = a;
    tmp -= b;
    return tmp;
  }

  double operator*(const Vector &a, const Vector &b) {
    return a.data[0]*b.data[0] + a.data[1]*b.data[1] + a.data[2]*b.data[2];
  }

  Vector operator*(const Vector &a, const double k) {
    return Vector(a.data[0]*k, a.data[1]*k, a.data[2]*k);
  }

  Vector operator*(const double k, const Vector &a) {
    return Vector(a.data[0]*k, a.data[1]*k, a.data[2]*k);
  }

  std::ostream &operator<<(std::ostream &out, const Vector &a) {
    out << "(" << a.data[0] << "," << a.data[1] << "," << a.data[2] << ")";
    return out;
  }

}  // namespace vector333
