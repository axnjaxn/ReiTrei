#ifndef _BPJ_REITREI_TRIANGLE_H
#define _BPJ_REITREI_TRIANGLE_H

#include "ray5object.h"

class Triangle : public Ray5Object {
protected:
  Vect4 a, b, c;

  //These variables are used in barycentric coordinates
  Vect4 p1, p2;
  Real d00, d01, d11, denom;
  
  Vect4 normal;
  
  void recompute();
  bool computeBary(const Vect4& P, Real& u, Real& v, Real& w);//Returns true if in triangle

public:
  Triangle(const Vect4& a, const Vect4& b, const Vect4& c);

  virtual Ray5Intersection intersectsUnit(const Vect4& O, const Vect4& D) const;

  void getBounds(Vect4* lower, Vect4* upper);
};

class InterpTriangle : public Triangle {
protected:
  Vect4 normal0, normal1, normal2;

public:
  InterpTriangle(const Vect4& a, const Vect4& b, const Vect4& c,
		 const Vect4& n, const Vect4& n1, const Vect4& n2);

  Ray5Intersection intersectsUnit(const Vect4& O, const Vect4& D) const;
};

#endif
