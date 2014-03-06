#ifndef _BPJ_REITREI5_CAMERA_H
#define _BPJ_REITREI5_CAMERA_H

#include "ray5object.h"
#include <cmath>

class Ray5Camera : public Ray5Object {
public:
  Real cellw, cellh;
  int pxw, pxh;

  virtual Ray5Intersection intersectsUnit(const Vect4& O, const Vect4& D) {return Ray5Intersection();}//Useless, but necessary override.

  Ray5Camera() {setScreen(100, 100, 1, 1, PI / 2);}

  void setScreen(int pxw, int pxh, Real xrange, Real yrange, Real fov) {
    this->pxw = pxw;
    this->pxh = pxh;
    cellw = xrange * 2 * tan(fov / 2) / pxw;
    cellh = yrange * 2 * tan(fov / 2) / pxh;
  }
  inline Vect4 getOrigin() const {return transformPoint(M, Vect4());}
  inline Vect4 getDirection(Real r, Real c) const {return transformDirection(M, Vect4((c  - pxw / 2) * cellw, -(r - pxh / 2) * cellh, 1).unit()).unit();}
};

#endif
