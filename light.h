#ifndef _BPJ_REITREI_LIGHT_H
#define _BPJ_REITREI_LIGHT_H

#include "mat4.h"

class Light {
public:
  Vect4 position, color;
  Real intensity, radius;
  bool falloff;//Does intensity fall off according to the inverse square law?

  Light() {color = Vect4(1, 1, 1); intensity = 1.0; radius = 0.0; falloff = 0;}
  Vect4 getColor() const {return color * intensity;}
};

#endif
