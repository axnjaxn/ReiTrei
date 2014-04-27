#include "mat4.h"

#include <cstring>

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

Vect3 Mat4::mult(const Vect3& v) const {
  Vect3 result;
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++)
      result[r] += v[c] * at(r, c);
  }
  return result;
}

Vect3 Mat4::mult(const Vect3& v, Real w) const {
  Vect3 result;
  Real W = 0.0;
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++)
      result[r] += v[c] * at(r, c);
    result[r] += w * at(r, 3);
  }
  for (int c = 0; c < 3; c++)
    W += v[c] * at(3, c);
  W += w * at(3, 3);
  return result / W;
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

Vect3 transformPoint(const Mat4& M, Vect3 v) {
  return M.mult(v, 1);
}

Vect3 transformDirection(const Mat4& M, Vect3 v) {
  return M.mult(v);
}
