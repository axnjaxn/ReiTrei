#ifndef _BPJ_REITREI_SHAPES_H
#define _BPJ_REITREI_SHAPES_H

#include "object.h"

class Sphere : public Object {
public:
  virtual Intersection intersectsUnit(const Vect3& O, const Vect3& D) const;
};

class Box : public Object {
public:
  virtual Intersection intersectsUnit(const Vect3& O, const Vect3& D) const;
};

class Cone : public Object {
public:
  bool has_caps;

  Cone() : Object() {has_caps = 1;}

  virtual Intersection intersectsUnit(const Vect3& O, const Vect3& D) const;
};

class Plane : public Object {
public:
  Vect3 A, N;
  Plane() : Object() {N = Vect3(0, 1, 0);}

  inline virtual bool infBounds() const {return 1;}
  virtual Intersection intersectsUnit(const Vect3& O, const Vect3& D) const;
};

#endif
