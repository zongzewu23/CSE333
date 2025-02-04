// Copyright (c) 2025 Z. Wu
// Name: Zongze Wu
// UW Email: zongzewu@uw.edu

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <iostream>

namespace vector333 {

class Vector {
 public:
  Vector();  // default constructor takes no parameter
  // constructor takes x, y, z
  Vector(const double x, const double y, const double z);
  Vector(const Vector& copyme);  // copy constructor
  ~Vector();  // destructor

  // // accessors, defined inline
  // double get_x() const {return x_;}
  // double get_y() const {return y_;}
  // double get_z() const {return z_;}


  // override the "=", "+=", and "-=" operators
  Vector &operator=(const Vector &a);
  Vector &operator+=(const Vector &a);
  Vector &operator-=(const Vector &a);
  // Vector &operator*=(const double k);

  // friend functions
  friend Vector operator+(const Vector &a, const Vector &b);
  friend Vector operator-(const Vector &a, const Vector &b);

  friend double operator*(const Vector &a, const Vector &b);

  friend Vector operator*(const Vector &a, const double k);
  friend Vector operator*(const double k, const Vector &a);

  friend std::ostream &operator<<(std::ostream &out, const Vector &a);
 private:
  // double x_, y_, z_;
  double *data;  // Member Variables
};  // class Vector

}  // namespace vector333

#endif  // _VECTOR_H_
