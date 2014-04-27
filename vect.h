#ifndef _BPJ_REITREI_VECT_H
#define _BPJ_REITREI_VECT_H

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

#endif
