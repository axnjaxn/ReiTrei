#ifndef _BPJ_REITREI4_SHAPES_H
#define _BPJ_REITREI4_SHAPES_H

#include "ray4object.h"
#include <cmath>

class Ray4Sphere : public Ray4Object {
public:
  virtual Ray4Intersection intersectsUnit(const Vect4& O, const Vect4& D) {
    REAL discriminant = dot(D, O) * dot(D, O) - dot(D, D) * (dot(O, O) - 1);
    if (discriminant < 0) return Ray4Intersection();

    REAL t1 = (dot(-D, O) + sqrt(discriminant)) / dot(D, D);
    REAL t2 = (dot(-D, O) - sqrt(discriminant)) / dot(D, D);
    REAL t = (t1 > 0 && (t2 <= 0 || t1 < t2))? t1 : t2;
    if (t <= 0) return Ray4Intersection();

    Vect4 P = O + t * D;
    return Ray4Intersection(this, t, P, P);
  }  
};

class Ray4Box : public Ray4Object {
public:
  virtual Ray4Intersection intersectsUnit(const Vect4& O, const Vect4& D) {
    REAL Tmin[3], Tmax[3];
    for (int i = 0; i < 3; i++)
      if (D[i] > 0) {
	Tmin[i] = (-1 - O[i]) / D[i];
	Tmax[i] = (1 - O[i]) / D[i];
      }
      else if (D[i] < 0) {
	Tmin[i] = (1 - O[i]) / D[i];
	Tmax[i] = (-1 - O[i]) / D[i];
      }
      else {
	Tmin[i] = (-1 - O[i]) / EPS;
	Tmax[i] = (1 - O[i]) / EPS;
      }
    
    REAL t0;
    if (Tmin[0] > Tmin[1]) t0 = Tmin[0];
    else t0 = Tmin[1];
    if (Tmin[2] > t0) t0 = Tmin[2];
    
    REAL t1;
    if (Tmax[0] < Tmax[1]) t1 = Tmax[0];
    else t1 = Tmax[1];
    if (Tmax[2] < t1) t1 = Tmax[2];

    if (t0 >= t1) return Ray4Intersection();

    REAL t = (t0 > 0 && (t1 <= 0 || t0 < t1))? t0 : t1;
    if (t <= 0) return Ray4Intersection();

    Vect4 P = O + t * D;
    Vect4 N;
    for (int i = 0; i < 3; i++)
      if (P[i] >= 1 - EPS) N[i] = 1;
      else if (P[i] <= -1 + EPS) N[i] = -1;

    return Ray4Intersection(this, t, P, N);
  }
};

class Ray4Cone : public Ray4Object {
public:
  REAL r0, r1;

  Ray4Cone() : Ray4Object() {r0 = r1 = 1;}

  virtual Ray4Intersection intersectsUnit(const Vect4& O, const Vect4& D) {
    REAL a = D[0] * D[0] + D[1] * D[1];
    REAL b = 2 * O[0] * D[0] + 2 * O[1] * D[1] - D[2] * (r1 - r0) / 2;
    REAL c = O[0] * O[0] + O[1] * O[1] - r0 - ((r1 - r0) * (O[2] + 1)) / 2;
    
    REAL discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return Ray4Intersection();
    
    REAL t1 = (-b + sqrt(discriminant)) / (2 * a);
    REAL t2 = (-b - sqrt(discriminant)) / (2 * a);
    REAL t = (t1 > 0 && (t2 <= 0 || t1 < t2))? t1 : t2;
    
    if (t <= 0) return Ray4Intersection();
    
    Vect4 P = O + t * D;
    if (P[1] < 0 || P[1] > 1) return Ray4Intersection();
    
    Vect4 N = Vect4(P[0], -P[1], P[2]).unit();
    
    Ray4Intersection result(this, t, P, N);
    return result;
  }
};

class Ray4Plane : public Ray4Object {
public:
  Vect4 A, N;
 Ray4Plane() : Ray4Object() {N = Vect4(0, 1, 0);}

  virtual bool infBounds() {return 1;}
  virtual Ray4Intersection intersectsUnit(const Vect4& O, const Vect4& D) {
    REAL t = dot(A - O, N) / dot(D, N);

    if (t > 0) return Ray4Intersection(this, t, O + t * D, N);
    else return Ray4Intersection();
  }
};

#endif
