#ifndef _BPJ_REITREI_TRIANGLE_H
#define _BPJ_REITREI_TRIANGLE_H

#include "object.h"

class Triangle : public Object {
protected:
  Vect3 a, b, c;

  //These variables are used in barycentric coordinates
  Vect3 p1, p2;
  Real d00, d01, d11, denom;
  
  Vect3 normal;
  
  void recompute();
  bool computeBary(const Vect3& P, Real& u, Real& v, Real& w) const ;//Returns true if in triangle

public:
  Triangle(const Vect3& a, const Vect3& b, const Vect3& c);

  virtual Intersection intersectsUnit(const Vect3& O, const Vect3& D) const;

  void getBounds(Vect3* lower, Vect3* upper);
};

class InterpTriangle : public Triangle {
protected:
  Vect3 normal0, normal1, normal2;

public:
  InterpTriangle(const Vect3& a, const Vect3& b, const Vect3& c,
		 const Vect3& n, const Vect3& n1, const Vect3& n2);

  Intersection intersectsUnit(const Vect3& O, const Vect3& D) const;
};

#endif
