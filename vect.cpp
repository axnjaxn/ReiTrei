#include "vect.h"
#include <cstring>

Vect4::Vect4() {memset(entries, 0, 4 * sizeof(Real));}

Vect4::Vect4(Real x, Real y, Real z, Real w) {
  entries[0] = x;
  entries[1] = y;
  entries[2] = z;
  entries[3] = w;
}

Vect4::Vect4(const Vect4& v) {*this = v;}

Vect4& Vect4::operator=(const Vect4& v) {memcpy(entries, v.entries, 4 * sizeof(Real)); return *this;}

Vect4 Vect4::operator+(const Vect4& v) const {
  Vect4 r;
  for (int i = 0; i < 4; i++) r[i] = entries[i] + v[i];
  return r;
}

Vect4 Vect4::operator-(const Vect4& v) const {
  Vect4 r;
  for (int i = 0; i < 4; i++) r[i] = entries[i] - v[i];
  return r;
}

Vect4 Vect4::operator*(Real r) const {
  Vect4 v;
  for (int i = 0; i < 4; i++)
    v[i] = entries[i] * r;
  return v;
}

Vect4 Vect4::operator/(Real r) const {
  Vect4 v;
  for (int i = 0; i < 4; i++)
    v[i] = entries[i] / r;
  return v;
}

Real dot(const Vect4& a, const Vect4& b) {
  Real r = 0.0;
  for (int i = 0; i < 4; i++) r += a[i] * b[i];
  return r;
}

Vect4 cross(const Vect4& a, const Vect4& b) {
  return Vect4(a[1] * b[2] - a[2] * b[1],
	       a[2] * b[0] - a[0] * b[2],
	       a[0] * b[1] - a[1] * b[0]);
}

Vect4 Vect4::multComp(const Vect4& v) const {
  Vect4 r;
  for (int i = 0; i < 4; i++)
    r[i] = entries[i] * v.entries[i];
  return r;
}

Vect4 Vect4::reciprocal() const {
  Vect4 r;
  for (int i = 0; i < 4; i++)
    r[i] = 1.0 / entries[i];
  return r;
}
