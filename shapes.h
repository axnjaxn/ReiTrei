#ifndef _BPJ_REITREI_SHAPES_H
#define _BPJ_REITREI_SHAPES_H

#include "object.h"

class Sphere : public Object {
public:
  virtual Intersection intersectsUnit(const Vect4& O, const Vect4& D) const;
};

class Box : public Object {
public:
  virtual Intersection intersectsUnit(const Vect4& O, const Vect4& D) const;
};

class Cone : public Object {
public:
  bool has_caps;

  Cone() : Object() {has_caps = 1;}

  virtual Intersection intersectsUnit(const Vect4& O, const Vect4& D) const;
};

class Plane : public Object {
public:
  Vect4 A, N;
  Plane() : Object() {N = Vect4(0, 1, 0);}

  inline virtual bool infBounds() const {return 1;}
  virtual Intersection intersectsUnit(const Vect4& O, const Vect4& D) const;
};

#endif
