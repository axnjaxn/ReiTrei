#ifndef _BPJ_REITREI_OBJECT_H
#define _BPJ_REITREI_OBJECT_H

#include "mat4.h"
#include <vector>

typedef enum {
  TRACE_NORMAL = 0,
  TRACE_SHADOW
} TraceMode;

class Material {
 public:
  bool shadowless, twosided;
  Vect4 ambient, reflective, refractive, diffuse;
  Real specular, shininess, refractive_index;

  Material() {
    shadowless = twosided = 0;
    specular = shininess = 0.0;
    refractive_index = 1.0;
  }
  Material(const Material& m) {*this = m;}
};

class Object; //fwd. decl.

class Intersection {
 public:
  const Object* obj;
  Real t;
  Vect4 P, N;

  Intersection() {obj = NULL; t = -1;}
  Intersection(const Object* obj, Real t, const Vect4& P, const Vect4& N) {
    this->obj = obj; this->t = t; this->P = P; this->N = N;
  }
  inline bool nearerThan(const Intersection& hit) {return (t > 0 && (hit.t < 0 || t < hit.t));}
};

class Object {
protected:
  Mat4 M, N;

public:
  Material material;
  
  inline Object() {M = N = Mat4::identity();}
  inline virtual ~Object() { }

  Intersection intersects(const Vect4& O, const Vect4& D) const;
  virtual Intersection intersectsUnit(const Vect4& O, const Vect4& D) const = 0;
  virtual inline bool infBounds() const {return 0;}
  virtual void getBounds(Vect4* lower, Vect4* upper);

  inline void scale(const Vect4& v) {
    M = Mat4::scaling(v) * M;
    N = N * Mat4::scaling(v.reciprocal());
  }
  inline void xrotate(Real th) {
    M = Mat4::xrotation(th) * M;
    N = N * Mat4::xrotation(th).transpose();
  }
  inline void yrotate(Real th) {
    M = Mat4::yrotation(th) * M;
    N = N * Mat4::yrotation(th).transpose();
  }
  inline void zrotate(Real th) {
    M = Mat4::zrotation(th) * M;
    N = N * Mat4::zrotation(th).transpose();
  }
  inline void translate(const Vect4& v) {
    M = Mat4::translation(v) * M;
    N = N * Mat4::translation(-v);
  }
};

class ObjectSet {
 protected:
  std::vector<Object*> objects;

 public:
  void release();
  inline void add(Object* obj) {objects.push_back(obj);}
  inline Object* operator[](int i) const {return objects[i];}
  inline int count() const {return objects.size();}

  int countBounded() const;
  Intersection intersect(const Vect4& O, const Vect4& D, TraceMode mode = TRACE_NORMAL) const;
  void getBounds(Vect4* L, Vect4* U) const;
};

#endif
