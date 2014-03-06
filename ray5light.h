#ifndef _BPJ_REITREI5_LIGHT_H
#define _BPJ_REITREI5_LIGHT_H

#include "mat4.h"

class Ray5Light {
public:
  Vect4 position, color;
  Real intensity, radius;

  Ray5Light() {color = Vect4(1, 1, 1); intensity = 1.0; radius = 0.0;}
};

#endif
