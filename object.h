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
  bool invisible, shadowless, twosided;
  Vect3 ambient, reflective, refractive, diffuse;
  Real specular, shininess, refractive_index;

  Material() {
    invisible = shadowless = twosided = 0;
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
  Vect3 P, N;

  Intersection() {obj = NULL; t = -1;}
  Intersection(const Object* obj, Real t, const Vect3& P, const Vect3& N) {
    this->obj = obj; this->t = t; this->P = P; this->N = N;
  }
  inline bool nearerThan(const Intersection& hit) {return (t > 0 && (hit.t < 0 || t < hit.t));}
};

class Modifier {
protected:
  Mat4 M, N;

public:
  inline Modifier() {M = N = Mat4::identity();}

  inline void scale(const Vect3& v) {
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
  inline void translate(const Vect3& v) {
    M = Mat4::translation(v) * M;
    N = N * Mat4::translation(-v);
  }
  inline void pinch(Real m, Real n) {
    if (m == 0) m = EPS;
    if (n == 0) n = EPS;
    M = Mat4::pinch(1 / m, 1 / n) * M;
    N = N * Mat4::pinch(m, n);
  }
  inline void applyModifier(const Modifier& m) {
    M = m.M * M;
    N = N * m.N;
  }

  Vect3 pointToObject(const Vect3&) const;
  Vect3 directionToObject(const Vect3&) const;
  Vect3 normalToObject(const Vect3&) const;
  Vect3 pointToWorld(const Vect3&) const;
  Vect3 directionToWorld(const Vect3&) const;
  Vect3 normalToWorld(const Vect3&) const;
};

class Object : public Modifier {
public:
  Material material;
  
  inline Object() : Modifier() { }
  inline virtual ~Object() { }

  Intersection intersects(const Vect3& O, const Vect3& D) const;
  virtual Intersection intersectsUnit(const Vect3& O, const Vect3& D) const = 0;
  virtual inline bool infBounds() const {return 0;}
  virtual void getBounds(Vect3* lower, Vect3* upper);
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
  Intersection intersect(const Vect3& O, const Vect3& D, TraceMode mode = TRACE_NORMAL) const;
  void getBounds(Vect3* L, Vect3* U) const;
};

#endif
