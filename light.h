#ifndef _BPJ_REITREI_LIGHT_H
#define _BPJ_REITREI_LIGHT_H

#include "mat4.h"

class Light {
public:
  Vect3 position, color;
  Real intensity, radius;
  bool falloff;//Does intensity fall off according to the inverse square law?

  Light() {color = Vect3(1, 1, 1); intensity = 1.0; radius = 0.0; falloff = 0;}
  Vect3 getColor() const {return color * intensity;}
};

#endif
