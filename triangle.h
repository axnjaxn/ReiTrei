#ifndef _BPJ_REITREI_TRIANGLE_H
#define _BPJ_REITREI_TRIANGLE_H

#include "ray5object.h"

class Triangle : public Ray5Object {
protected:
  Vect4 a, b, c;

  //These variables are used in barycentric coordinates
  Vect4 p1, p2;
  float d00, d01, d11, denom;
  
  Vect4 normal;
  
  void recompute();

public:
  Triangle(const Vect4& a, const Vect4& b, const Vect4& c);

  Ray5Intersection intersectsUnit(const Vect4& O, const Vect4& D) const;

  void getBounds(Vect4* lower, Vect4* upper);
  inline void setNormal(const Vect4& normal) {this->normal = normal;}
};

#endif
