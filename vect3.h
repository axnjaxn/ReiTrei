#ifndef _BPJ_REITREI_VECT_H
#define _BPJ_REITREI_VECT_H

#include <cmath>

typedef double Real;
const Real PI = 3.14159265358979;
const Real EPS = 1e-10;

class Vect3 {
protected:
  Real entries[3];

public:
  Vect3();
  Vect3(Real, Real, Real);
  Vect3(const Vect3&);

  Vect3& operator=(const Vect3&);

  inline Vect3 operator-() const {return *this * -1;}
  inline Real operator*(const Vect3& v) const {return dot(*this, v);}
  Vect3 operator+(const Vect3&) const;
  inline Vect3& operator+=(const Vect3& v) {*this = *this + v; return *this;}
  Vect3 operator-(const Vect3&) const;
  inline Vect3& operator-=(const Vect3& v) {*this = *this + v; return *this;}
  Vect3 operator*(Real) const;
  Vect3 operator/(Real) const;
  inline Real& operator[](int i) {return entries[i];}
  inline Real operator[](int i) const {return entries[i];}

  friend Real dot(const Vect3& a, const Vect3& b);
  friend Vect3 cross(const Vect3& a, const Vect3& b);

  inline Real length() const {return sqrt(sqLength());}
  inline Real sqLength() const {return *this * *this;}
  inline Vect3 unit() const {return *this / length();}
  Vect3 multComp(const Vect3&) const;
  Vect3 reciprocal() const;
};

inline Vect3 operator*(Real r, const Vect3& v) {return v * r;}
inline bool nonzero(const Vect3& v) {return v[0] || v[1] || v[2];}
inline Real distance(const Vect3& a, const Vect3& b) {return (b - a).length();}
inline Real sqDistance(const Vect3& a, const Vect3& b) {return (b - a).sqLength();}

#endif
