#include "mat4.h"
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

Mat4::Mat4() {
  memset(entries, 0, sizeof(Real) * 16);
}

Mat4::Mat4(const Mat4& m) {*this = m;}

Mat4& Mat4::operator=(const Mat4& m) {
  memcpy(entries, m.entries, 16 * sizeof(Real));
  return *this;
}

Mat4 Mat4::operator+(const Mat4& m) const {
  Mat4 result;
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      result.at(r, c) = at(r, c) + m.at(r, c);
  return result;
}

Mat4 Mat4::operator-(const Mat4& m) const {
  Mat4 result;
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      result.at(r, c) = at(r, c) - m.at(r, c);
  return result;
}

Mat4 Mat4::operator*(const Mat4& m) const {
  Mat4 result;
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      for (int i = 0; i < 4; i++)
	result.at(r, c) += at(r, i) * m.at(i, c);
  return result;
}

Vect4 Mat4::operator*(const Vect4& v) const {
  Vect4 result;
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      result[r] += v[c] * at(r, c);//Test this
  return result;
}

Mat4 Mat4::operator*(Real f) const {
  Mat4 result;
  for (int i = 0; i < 16; i++) result.entries[i] = entries[i] * f;
  return result;
}

Mat4 Mat4::operator/(Real f) const {
  Mat4 result;
  for (int i = 0; i < 16; i++) result.entries[i] = entries[i] / f;
  return result;
}

Real& Mat4::at(int r, int c) {
#ifdef USE_PARACHUTES
  if (!entries || r < 0 || c < 0 || r >= 4 || c >= 4) parachute("Out of bounds.");
#endif
  return entries[r * 4 + c];
}

Real Mat4::at(int r, int c) const {
#ifdef USE_PARACHUTES
  if (!entries || r < 0 || c < 0 || r >= 4 || c >= 4) parachute("Out of bounds.");
#endif
  return entries[r * 4 + c];
}

Mat4 Mat4::transpose() const {
  Mat4 result;
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      result.at(r, c) = at(c, r);
  return result;
}

Mat4 Mat4::reciprocal() const {
  Mat4 result;
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      result.at(r, c) = 1.0 / at(r, c);
  return result;
}

Mat4 Mat4::identity() {
  Mat4 m;
  m.entries[0] = m.entries[5] = m.entries[10] = m.entries[15] = 1.0;
  return m;
}
 
Mat4 Mat4::xrotation(Real th) {
  Mat4 Rx = Mat4::identity();
  Rx.at(1, 1) = cos(th);
  Rx.at(2, 2) = cos(th);
  Rx.at(1, 2) = -sin(th);
  Rx.at(2, 1) = sin(th);
  return Rx;
}

Mat4 Mat4::yrotation(Real th) {
  Mat4 Ry = Mat4::identity();
  Ry.at(0, 0) = cos(th);
  Ry.at(2, 2) = cos(th);
  Ry.at(2, 0) = -sin(th);
  Ry.at(0, 2) = sin(th);
  return Ry;
}

Mat4 Mat4::zrotation(Real th) {
  Mat4 Rz = Mat4::identity();
  Rz.at(0, 0) = cos(th);
  Rz.at(1, 1) = cos(th);
  Rz.at(0, 1) = -sin(th);
  Rz.at(1, 0) = sin(th);
  return  Rz;
}

Mat4 Mat4::translation(Real tx, Real ty, Real tz) {
  Mat4 result = Mat4::identity();
  result.at(0, 3) = tx;
  result.at(1, 3) = ty;
  result.at(2, 3) = tz;
  return result;
}

Mat4 Mat4::scaling(Real sx, Real sy, Real sz) {
  Mat4 result = Mat4::identity();
  result.at(0, 0) = sx;
  result.at(1, 1) = sy;
  result.at(2, 2) = sz;
  return result;
}

Mat4 Mat4::pinch(Real m, Real n) {
  Mat4 result;
  result.at(0, 0) = result.at(2, 2) = 2;
  result.at(1, 1) = result.at(3, 3) = m + n;
  result.at(1, 3) = result.at(3, 1) = m - n;
  return result;
}
