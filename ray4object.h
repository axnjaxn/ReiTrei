#ifndef _BPJ_REITREI4_OBJECT_H
#define _BPJ_REITREI4_OBJECT_H

#include "mat4.h"
#include <cstdlib>

typedef Real REAL;//del

class Ray4Material {
 public:
  bool shadowless;
  bool twosided;
  Vect4 ambient;
  Vect4 reflective;
  Vect4 diffuse;
  Real specular, shininess;

  Ray4Material() {
    shadowless = twosided = 0;
    specular = shininess = 0.0;
  }
  Ray4Material(const Ray4Material& m) {*this = m;}
};

class Ray4Object;

class Ray4Intersection {
 public:
  Ray4Object* obj;
  Real t;
  Vect4 P, N;

  Ray4Intersection() {obj = NULL; t = -1;}

  Ray4Intersection(Ray4Object* obj, Real t, const Vect4& P, const Vect4& N) {
    this->obj = obj;
    this->t = t;
    this->P = P;
    this->N = N;
  }
  inline bool nearerThan(const Ray4Intersection& hit) {return (t > 0 && (hit.t < 0 || t < hit.t));}
};

class Ray4Object {
protected:
  Mat4 M, N;

public:
  Ray4Material material;
  
  inline Ray4Object() {M = N = identity();}

  Ray4Intersection intersects(const Vect4& O, const Vect4& D) {
    Ray4Intersection result = intersectsUnit(transformPoint(N, O), transformDirection(N, D));
    if (result.t > 0) {
      result.P = transformPoint(M, result.P);
      result.N = transformDirection(N.transpose(), result.N);
    }
    return result;
  }
  virtual Ray4Intersection intersectsUnit(const Vect4& O, const Vect4& D) = 0;
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
    M = scaling(v) * M;
    N = N * scaling(v.reciprocal());
  }
  inline void xrotate(Real th) {
    M = xrotation(th) * M;
    N = N * xrotation(th).transpose();
  }
  inline void yrotate(Real th) {
    M = yrotation(th) * M;
    N = N * yrotation(th).transpose();
  }
  inline void zrotate(Real th) {
    M = zrotation(th) * M;
    N = N * zrotation(th).transpose();
  }
  inline void translate(const Vect4& v) {
    M = translation(v) * M;
    N = N * translation(-v);
  }
};

#include <vector>

class Ray4ObjectSet {
 protected:
  std::vector<Ray4Object*> objects;

 public:
  void release() {
    for (int i = 0; i < count(); i++) delete (*this)[i];
    objects.clear();
  }
  inline void add(Ray4Object* obj) {objects.push_back(obj);}
  inline Ray4Object* operator[](int i) const {return objects[i];}
  int count() const {return objects.size();}
  int countBounded() const {
    int sum = 0;
    for (int i = 0; i < count(); i++) if (!objects[i]->infBounds()) sum++;
    return sum;
  }

  Ray4Intersection intersect(const Vect4& O, const Vect4& D) const {
    Ray4Intersection nearest, next;
    
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
