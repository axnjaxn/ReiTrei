#ifndef _BPJ_REITREI5_MAT4_H
#define _BPJ_REITREI5_MAT4_H

#include <cstdlib>
#include <cmath>

typedef double Real;
const Real PI = 3.14159265358979;
const Real EPS = 1e-10;

class Vect4 {
protected:
  Real entries[4];

public:
  Vect4();
  Vect4(Real, Real, Real, Real = 0);
  Vect4(const Vect4&);

  Vect4& operator=(const Vect4&);

  inline Vect4 operator-() const {return *this * -1;}
  inline Real operator*(const Vect4& v) const {return dot(*this, v);}
  Vect4 operator+(const Vect4&) const;
  inline Vect4& operator+=(const Vect4& v) {*this = *this + v; return *this;}
  Vect4 operator-(const Vect4&) const;
  inline Vect4& operator-=(const Vect4& v) {*this = *this + v; return *this;}
  Vect4 operator*(Real) const;
  Vect4 operator/(Real) const;
  inline Real& operator[](int i) {return entries[i];}
  inline Real operator[](int i) const {return entries[i];}

  friend Real dot(const Vect4& a, const Vect4& b);
  friend Vect4 cross(const Vect4& a, const Vect4& b);

  inline Real length() const {return sqrt(sqLength());}
  inline Real sqLength() const {return *this * *this;}
  inline Vect4 unit() const {return *this / length();}
  Vect4 multComp(const Vect4&) const;
  Vect4 reciprocal() const;
};

inline Vect4 operator*(Real r, const Vect4& v) {return v * r;}
inline bool nonzero(const Vect4& v) {return v[0] || v[1] || v[2] || v[3];}
inline Real distance(const Vect4& a, const Vect4& b) {return (b - a).length();}
inline Real sqDistance(const Vect4& a, const Vect4& b) {return (b - a).sqLength();}

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

  inline static Mat4 translation(const Vect4& v) {return translation(v[0], v[1], v[2]);}
  inline static Mat4 scaling(const Vect4& v) {return scaling(v[0], v[1], v[2]);}
};

inline Mat4 operator*(Real f, const Mat4& m) {return m * f;}

inline Vect4 transformPoint(const Mat4& M, Vect4 v) {
  v[3] = 1;
  v = M * v;
  v[3] = 0;
  return v;
}

inline Vect4 transformDirection(const Mat4& M, Vect4 v) {
  v[3] = 0;
  v = M * v;
  v[3] = 0;
  return v;
}

#endif
