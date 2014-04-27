#ifndef _BPJ_REITREI_MAT4_H
#define _BPJ_REITREI_MAT4_H

#include "vect.h"

#include <cstdlib>
#include <cmath>

class Mat4 {
protected:
  Real entries[16];

public:
  Mat4();
  Mat4(const Mat4&);

  Mat4& operator=(const Mat4&);

  Mat4 operator+(const Mat4&) const;
  inline Mat4& operator+=(const Mat4& m) {return *this = *this + m;}
  Mat4 operator-(const Mat4&) const;
  inline Mat4& operator-=(const Mat4& m) {return *this = *this - m;}
  inline Mat4 operator-() const {return *this * -1;}
  Mat4 operator*(const Mat4&) const;
  Vect4 operator*(const Vect4&) const;
  Mat4 operator*(Real) const;
  Mat4 operator/(Real) const;

  Real& at(int, int);
  Real at(int, int) const;

  Mat4 transpose() const;
  Mat4 reciprocal() const;
  
  static Mat4 identity();  
  static Mat4 xrotation(Real);
  static Mat4 yrotation(Real);
  static Mat4 zrotation(Real);
  static Mat4 translation(Real, Real, Real);
  static Mat4 scaling(Real, Real, Real);
  static Mat4 pinch(Real, Real);

  inline static Mat4 translation(const Vect4& v) {return translation(v[0], v[1], v[2]);}
  inline static Mat4 scaling(const Vect4& v) {return scaling(v[0], v[1], v[2]);}
};

inline Mat4 operator*(Real f, const Mat4& m) {return m * f;}

Vect4 transformPoint(const Mat4& M, Vect4 v);
Vect4 transformDirection(const Mat4& M, Vect4 v);

#endif
