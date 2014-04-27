#include "vect3.h"
#include <cstring>

Vect3::Vect3() {memset(entries, 0, 3 * sizeof(Real));}

Vect3::Vect3(Real x, Real y, Real z) {
  entries[0] = x;
  entries[1] = y;
  entries[2] = z;
}

Vect3::Vect3(const Vect3& v) {*this = v;}

Vect3& Vect3::operator=(const Vect3& v) {memcpy(entries, v.entries, 3 * sizeof(Real)); return *this;}

Vect3 Vect3::operator+(const Vect3& v) const {
  Vect3 r;
  for (int i = 0; i < 3; i++) r[i] = entries[i] + v[i];
  return r;
}

Vect3 Vect3::operator-(const Vect3& v) const {
  Vect3 r;
  for (int i = 0; i < 3; i++) r[i] = entries[i] - v[i];
  return r;
}

Vect3 Vect3::operator*(Real r) const {
  Vect3 v;
  for (int i = 0; i < 3; i++)
    v[i] = entries[i] * r;
  return v;
}

Vect3 Vect3::operator/(Real r) const {
  Vect3 v;
  for (int i = 0; i < 3; i++)
    v[i] = entries[i] / r;
  return v;
}

Real dot(const Vect3& a, const Vect3& b) {
  Real r = 0.0;
  for (int i = 0; i < 3; i++) r += a[i] * b[i];
  return r;
}

Vect3 cross(const Vect3& a, const Vect3& b) {
  return Vect3(a[1] * b[2] - a[2] * b[1],
	       a[2] * b[0] - a[0] * b[2],
	       a[0] * b[1] - a[1] * b[0]);
}

Vect3 Vect3::multComp(const Vect3& v) const {
  Vect3 r;
  for (int i = 0; i < 3; i++)
    r[i] = entries[i] * v.entries[i];
  return r;
}

Vect3 Vect3::reciprocal() const {
  Vect3 r;
  for (int i = 0; i < 3; i++)
    r[i] = 1.0 / entries[i];
  return r;
}
