#ifndef _BPJ_REITREI_TRIANGLE_H
#define _BPJ_REITREI_TRIANGLE_H

#include "object.h"

class Triangle : public Object {
protected:
  Vect4 a, b, c;

  //These variables are used in barycentric coordinates
  Vect4 p1, p2;
  Real d00, d01, d11, denom;
  
  Vect4 normal;
  
  void recompute();
  bool computeBary(const Vect4& P, Real& u, Real& v, Real& w) const ;//Returns true if in triangle

public:
  Triangle(const Vect4& a, const Vect4& b, const Vect4& c);

  virtual Intersection intersectsUnit(const Vect4& O, const Vect4& D) const;

  void getBounds(Vect4* lower, Vect4* upper);
};

class InterpTriangle : public Triangle {
protected:
  Vect4 normal0, normal1, normal2;

public:
  InterpTriangle(const Vect4& a, const Vect4& b, const Vect4& c,
		 const Vect4& n, const Vect4& n1, const Vect4& n2);

  Intersection intersectsUnit(const Vect4& O, const Vect4& D) const;
};

#endif
