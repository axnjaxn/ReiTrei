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

Triangle::Triangle(const Vect4& a, const Vect4& b, const Vect4& c) : Ray5Object() {
  this->a = a;
  this->b = b;
  this->c = c;
  recompute();
}

Ray5Intersection Triangle::intersectsUnit(const Vect4& O, const Vect4& D) const {
  Real t = dot(a - O, normal) / dot(D, normal);

  if (t > 0) {
    //An application of Cramer's rule
    //Adapted from Christer Ericson's Real Time Collision Detection
    Vect4 P = O + t * D;
    Vect4 p = P - a;

    float d20 = dot(p, p1);
    float d21 = dot(p, p2);

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0 - v - w;

    if (0.0 <= u && u <= 1.0 && 0.0 <= v && v <= 1.0 && 0.0 <= w && w <= 1.0) 
      return Ray5Intersection(this, t, P, normal);
  }
    
  return Ray5Intersection();
}
  
void Triangle::getBounds(Vect4* lower, Vect4* upper) {
  *lower = *upper = a;
  for (int i = 0; i < 3; i++) {
    if (b[i] < (*lower)[i]) (*lower)[i] = b[i];
    else if (b[i] > (*upper)[i]) (*upper)[i] = b[i];
    if (c[i] < (*lower)[i]) (*lower)[i] = c[i];
    else if (c[i] > (*upper)[i]) (*upper)[i] = c[i];
  }
}
