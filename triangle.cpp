#include "triangle.h"
#include <cstdio>

void Triangle::recompute() {
  p1 = b - a;
  p2 = c - a;
  normal = cross(p1, p2).unit();
  d00 = dot(p1, p1);
  d01 = dot(p1, p2);
  d11 = dot(p2, p2);
  denom = d00 * d11 - d01 * d01;
}

bool Triangle::computeBary(const Vect3& P, Real& u, Real& v, Real& w) const {
  /* 
   * An application of Cramer's rule
   * Adapted from Christer Ericson's Real Time Collision Detection
   */
  Vect3 p = P - a;
  Real d20 = dot(p, p1);
  Real d21 = dot(p, p2);
  
  v = (d11 * d20 - d01 * d21) / denom;
  w = (d00 * d21 - d01 * d20) / denom;
  u = 1.0 - v - w;

  return (v >= 0.0 && w >= 0.0 && v + w <= 1.0);
}

Triangle::Triangle(const Vect3& a, const Vect3& b, const Vect3& c) : Object() {
  this->a = a;
  this->b = b;
  this->c = c;
  recompute();
}

Intersection Triangle::intersectsUnit(const Vect3& O, const Vect3& D) const {
  Real t = dot(a - O, normal) / dot(D, normal), u, v, w;

  if (t > 0) {
    Vect3 P = O + t * D;
    if (computeBary(P, u, v, w))
      return Intersection(this, t, P, normal);
  }
  
  return Intersection();
}
  
void Triangle::getBounds(Vect3* lower, Vect3* upper) {
  Vect3 a = transformPoint(M, this->a);
  Vect3 b = transformPoint(M, this->b);
  Vect3 c = transformPoint(M, this->c);
  *lower = *upper = a;
  for (int i = 0; i < 3; i++) {
    if (b[i] < (*lower)[i]) (*lower)[i] = b[i];
    else if (b[i] > (*upper)[i]) (*upper)[i] = b[i];
    if (c[i] < (*lower)[i]) (*lower)[i] = c[i];
    else if (c[i] > (*upper)[i]) (*upper)[i] = c[i];
  }
}

InterpTriangle::InterpTriangle(const Vect3& a, const Vect3& b, const Vect3& c,
			       const Vect3& n, const Vect3& n1, const Vect3& n2)
  : Triangle(a, b, c) {
  normal0 = n.unit();
  normal1 = n1.unit();
  normal2 = n2.unit();
}

Intersection InterpTriangle::intersectsUnit(const Vect3& O, const Vect3& D) const {
  Real t = dot(a - O, normal) / dot(D, normal), u, v, w;

  if (t > 0) {
    Vect3 P = O + t * D;
    if (computeBary(P, u, v, w))
      return Intersection(this, t, P, u * normal0 + v * normal1 + w * normal2);
  }

  return Intersection();
}
