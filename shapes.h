#ifndef _BPJ_REITREI_SHAPES_H
#define _BPJ_REITREI_SHAPES_H

#include "object.h"
#include <cmath>

class Ray5Sphere : public Object {
public:
  virtual Intersection intersectsUnit(const Vect4& O, const Vect4& D) const {
    Real discriminant = dot(D, O) * dot(D, O) - dot(D, D) * (dot(O, O) - 1);
    if (discriminant < 0) return Intersection();

    Real t1 = (dot(-D, O) + sqrt(discriminant)) / dot(D, D);
    Real t2 = (dot(-D, O) - sqrt(discriminant)) / dot(D, D);
    Real t = (t1 > 0 && (t2 <= 0 || t1 < t2))? t1 : t2;
    if (t <= 0) return Intersection();

    Vect4 P = O + t * D;
    return Intersection(this, t, P, P);
  }  
};

class Ray5Box : public Object {
public:
  virtual Intersection intersectsUnit(const Vect4& O, const Vect4& D) const {
    Real Tmin[3], Tmax[3];
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
    
    Real t0;
    if (Tmin[0] > Tmin[1]) t0 = Tmin[0];
    else t0 = Tmin[1];
    if (Tmin[2] > t0) t0 = Tmin[2];
    
    Real t1;
    if (Tmax[0] < Tmax[1]) t1 = Tmax[0];
    else t1 = Tmax[1];
    if (Tmax[2] < t1) t1 = Tmax[2];

    if (t0 >= t1) return Intersection();

    Real t = (t0 > 0 && (t1 <= 0 || t0 < t1))? t0 : t1;
    if (t <= 0) return Intersection();

    Vect4 P = O + t * D;
    Vect4 N;
    for (int i = 0; i < 3; i++)
      if (P[i] >= 1 - EPS) N[i] = 1;
      else if (P[i] <= -1 + EPS) N[i] = -1;

    return Intersection(this, t, P, N);
  }
};

class Ray5Plane : public Object {
public:
  Vect4 A, N;
  Ray5Plane() : Object() {N = Vect4(0, 1, 0);}

  virtual bool infBounds() {return 1;}
  virtual Intersection intersectsUnit(const Vect4& O, const Vect4& D) const {
    Real t = dot(A - O, N) / dot(D, N);

    if (t > 0) return Intersection(this, t, O + t * D, N);
    else return Intersection();
  }
};

#endif
