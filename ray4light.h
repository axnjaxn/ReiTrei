#ifndef _BPJ_REITREI4_LIGHT_H
#define _BPJ_REITREI4_LIGHT_H

#include "mat4.h"

class Ray4Light {
public:
  Vect4 position;
  Vect4 color;
  REAL intensity;
  REAL radius;

  Ray4Light() {color = Vect4(1, 1, 1); intensity = 1.0; radius = 0.0;}
};

#endif
