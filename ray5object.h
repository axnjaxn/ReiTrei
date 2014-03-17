#ifndef _BPJ_REITREI5_OBJECT_H
#define _BPJ_REITREI5_OBJECT_H

#include "mat4.h"
#include <cstdlib>

class Ray5Material {
 public:
  bool shadowless, twosided;
  Vect4 ambient, reflective, diffuse;
  Real specular, shininess;

  Ray5Material() {
    shadowless = twosided = 0;
    specular = shininess = 0.0;
  }
  Ray5Material(const Ray5Material& m) {*this = m;}
};

class Ray5Object;

class Ray5Intersection {
 public:
  Ray5Object* obj;
  Real t;
  Vect4 P, N;

  Ray5Intersection() {obj = NULL; t = -1;}
  Ray5Intersection(Ray5Object* obj, Real t, const Vect4& P, const Vect4& N) {
    this->obj = obj; this->t = t; this->P = P; this->N = N;
  }
  inline bool nearerThan(const Ray5Intersection& hit) {return (t > 0 && (hit.t < 0 || t < hit.t));}
};

class Ray5Object {
protected:
  Mat4 M, N;

public:
  Ray5Material material;
  
  inline Ray5Object() {M = N = Mat4::identity();}
  inline virtual ~Ray5Object() { }

  Ray5Intersection intersects(const Vect4& O, const Vect4& D) {
    Ray5Intersection result = intersectsUnit(transformPoint(N, O), transformDirection(N, D));
    if (result.t > 0) {
      result.P = transformPoint(M, result.P);
      result.N = transformDirection(N.transpose(), result.N);
    }
    return result;
  }
  virtual Ray5Intersection intersectsUnit(const Vect4& O, const Vect4& D) = 0;
  virtual inline bool infBounds() {return 0;}
  virtual void getBounds(Vect4* lower, Vect4* upper) {
    Vect4 corners[8];
    corners[0] = transformPoint(M, Vect4(-1, -1, -1));
    corners[1] = transformPoint(M, Vect4(-1, -1, 1));
    corners[2] = transformPoint(M, Vect4(-1, 1, -1));
    corners[3] = transformPoint(M, Vect4(-1, 1, 1));
    corners[4] = transformPoint(M, Vect4(1, -1, -1));
    corners[5] = transformPoint(M, Vect4(1, -1, 1));
    corners[6] = transformPoint(M, Vect4(1, 1, -1));
    corners[7] = transformPoint(M, Vect4(1, 1, 1));

    Vect4 U, L;
    U = L = corners[0];
    for (int i = 1; i < 8; i++) {
      for (int j = 0; j < 3; j++) {
	if (corners[i][j] > U[j]) U[j] = corners[i][j];
	if (corners[i][j] < L[j]) L[j] = corners[i][j];
      }
    }

    *upper = U; *lower = L;
  }

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

#include <vector>

class Ray5ObjectSet {
 protected:
  std::vector<Ray5Object*> objects;

 public:
  void release() {
    for (int i = 0; i < objects.size(); i++) delete objects[i];
    objects.clear();
  }
  inline void add(Ray5Object* obj) {objects.push_back(obj);}
  inline Ray5Object* operator[](int i) const {return objects[i];}
  int count() const {return objects.size();}
  int countBounded() const {
    int sum = 0;
    for (int i = 0; i < count(); i++) if (!objects[i]->infBounds()) sum++;
    return sum;
  }

  Ray5Intersection intersect(const Vect4& O, const Vect4& D) const {
    Ray5Intersection nearest, next;
    
    for (int i = 0; i < count(); i++) {
      next = objects[i]->intersects(O, D);
      if (next.nearerThan(nearest)) nearest = next;
    }
    
    return nearest;
  }
  void getBounds(Vect4* L, Vect4* U) const {
    Vect4 l, u, li, ui;
    bool initial = 1;
    for (int i = 0; i < count(); i++) {
      if (objects[i]->infBounds()) continue;
      objects[i]->getBounds(&li, &ui);
      if (initial) {
	l = li;
	u = ui;
	initial = 0;
      }
      else
	for (int j = 0; j < 3; j++) {
	  if (li[j] < l[j]) l[j] = li[j];
	  if (ui[j] > u[j]) u[j] = ui[j];
	}
    }
    *L = l;
    *U = u;
  }
};

#endif
